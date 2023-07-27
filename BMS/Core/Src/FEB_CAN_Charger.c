// ********************************** Includes & External **********************************

#include "FEB_CAN_Charger.h"

extern CAN_TxHeaderTypeDef FEB_CAN_TxHeader;
extern uint8_t FEB_CAN_TxData[8];
extern uint32_t FEB_CAN_TxMailbox;
extern uint8_t FEB_BMS_Shutdown_State;

// ********************************** CAN Configuration **********************************

static FEB_CAN_CHARGER_BMS_MESSAGE_TYPE FEB_CAN_Charger_BMS_Message;
static FEB_CAN_CHAERGER_CHARGER_MESSAGE_TYPE FEB_CAN_Charger_Charger_Message;
static uint8_t FEB_CAN_Charger_Rx_Flag;

// ********************************** Charger Configuration **********************************

static uint8_t FEB_CAN_Charger_Stop_Charge_Bool = 0;	// 0 (actively charging), 1 (finished charging)

// ********************************** Functions **********************************

// ******************** Startup ********************

void FEB_CAN_Charger_Init() {
	if (FEB_CAN_CHARGER_STATE == 0) {
		return;
	}
	FEB_CAN_Charger_BMS_Message.max_voltage_dV = (uint16_t) (FEB_LTC6811_CELL_MAX_VOLTAGE * FEB_LTC6811_NUM_CELLS_PER_BANK * FEB_LTC6811_NUM_BANKS * 10);
	FEB_CAN_Charger_BMS_Message.max_current_dA = (uint16_t) (FEB_CAN_CHARGER_MAX_CHARGING_CURRENT * 10);
	FEB_CAN_Charger_BMS_Message.control = 0;
}

// ******************** CAN ********************

uint8_t FEB_CAN_Charger_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment, uint8_t bank) {
	CAN_FilterTypeDef my_can_filter_config;

	my_can_filter_config.FilterActivation = CAN_FILTER_ENABLE;
	my_can_filter_config.FilterBank = bank;
	my_can_filter_config.FilterFIFOAssignment = FIFO_Assignment;
	my_can_filter_config.FilterIdHigh = FEB_CAN_CHARGER_CHARGER_ID >> 13; 			// First 16 bits
	my_can_filter_config.FilterIdLow = (FEB_CAN_CHARGER_CHARGER_ID & 0x1FFF) << 3;	// Last 13 bits
	my_can_filter_config.FilterMaskIdHigh = 0xFFFF; 								// Mask first 16 bits
	my_can_filter_config.FilterMaskIdLow = 0xFFF8;									// Mask last 13 bits
	my_can_filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	my_can_filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	my_can_filter_config.SlaveStartFilterBank = 27;

	if (HAL_CAN_ConfigFilter(hcan, &my_can_filter_config)) {
		FEB_BMS_Shutdown_Initiate("Invalid charger CAN filter configuration");
	}

	return ++bank;
}

void FEB_CAN_Charger_Transmit(CAN_HandleTypeDef* hcan) {
	// Initialize transmission header
	FEB_CAN_TxHeader.DLC = 8;
	FEB_CAN_TxHeader.ExtId = FEB_CAN_CHARGER_BMS_ID;
	FEB_CAN_TxHeader.IDE = CAN_ID_EXT;
	FEB_CAN_TxHeader.RTR = CAN_RTR_DATA;
	FEB_CAN_TxHeader.TransmitGlobalTime = DISABLE;

	// Copy data to Tx buffer
	FEB_CAN_TxData[0] = FEB_CAN_Charger_BMS_Message.max_voltage_dV >> 8;
	FEB_CAN_TxData[1] = FEB_CAN_Charger_BMS_Message.max_voltage_dV & 0xFF;
	FEB_CAN_TxData[2] = FEB_CAN_Charger_BMS_Message.max_current_dA >> 8;
	FEB_CAN_TxData[3] = FEB_CAN_Charger_BMS_Message.max_current_dA & 0xFF;
	FEB_CAN_TxData[4] = FEB_CAN_Charger_BMS_Message.control;
	FEB_CAN_TxData[5] = 0;
	FEB_CAN_TxData[6] = 0;
	FEB_CAN_TxData[7] = 0;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(hcan, &FEB_CAN_TxHeader, FEB_CAN_TxData, &FEB_CAN_TxMailbox) != HAL_OK) {
		FEB_BMS_Shutdown_Initiate("CAN charger transmission error");
	}
}

void FEB_CAN_Charger_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t RxData[]) {
	switch(pHeader->ExtId) {
	    case FEB_CAN_CHARGER_CHARGER_ID:
	    	FEB_CAN_Charger_Charger_Message.operating_voltage_dV = (RxData[0] << 8) + RxData[1];
	    	FEB_CAN_Charger_Charger_Message.operating_current_dA = (RxData[2] << 8) + RxData[3];
	    	FEB_CAN_Charger_Charger_Message.status = RxData[4];
			FEB_CAN_Charger_Validate_Status(RxData[4]);
			break;
	}
}

// ******************** Process Data ********************

void FEB_CAN_Charger_Validate_Status(uint8_t status) {
	// Failure bits: 0 (no error), 1 (error)
	uint8_t hardware_failure 			= (status >> 7) & 0b1;
	uint8_t temperature_failure 		= (status >> 6) & 0b1;
	uint8_t input_voltage_failure 		= (status >> 5) & 0b1;
	uint8_t starting_state_failure 		= (status >> 4) & 0b1;
	uint8_t communication_state_failure = (status >> 3) & 0b1;

	if (hardware_failure == 1) {
		FEB_BMS_Shutdown_Initiate("CAN charger hardware_failure status error");
	}
	if (temperature_failure == 1) {
			FEB_BMS_Shutdown_Initiate("CAN charger temperature_failure status error");
		}
	if (input_voltage_failure == 1) {
			FEB_BMS_Shutdown_Initiate("CAN charger input_voltage_failure status error");
		}
	if (starting_state_failure == 1) {
			FEB_BMS_Shutdown_Initiate("CAN charger starting_state_failure status error");
		}
	if (communication_state_failure == 1) {
			FEB_BMS_Shutdown_Initiate("CAN charger communication_state_failure status error");
		}
}

void FEB_CAN_Charger_Process(CAN_HandleTypeDef* hcan) {
	if (FEB_CAN_CHARGER_STATE == 0 || FEB_CAN_Charger_Stop_Charge_Bool == 1) {
		return;
	} else if (FEB_BMS_Shutdown_State == 1) {
		FEB_CAN_Charger_Stop_Charge(hcan);
	}
	if (FEB_CAN_Charger_Rx_Flag == 1) {
		FEB_CAN_Charger_Rx_Flag = 0;
		float operating_current = (float) FEB_CAN_Charger_Charger_Message.operating_current_dA * 0.1;
		if (operating_current > FEB_CAN_CHARGER_MAX_CHARGING_CURRENT) {
			FEB_CAN_Charger_Stop_Charge(hcan);
			FEB_BMS_Shutdown_Initiate("CAN charger over current");
		}
	}
	if (FEB_LTC6811_Cells_Charged() == 1) {
		FEB_CAN_Charger_Stop_Charge(hcan);
	} else {
		FEB_CAN_Charger_Transmit(hcan);
	}
}

void FEB_CAN_Charger_Stop_Charge(CAN_HandleTypeDef* hcan) {
	FEB_CAN_Charger_Stop_Charge_Bool = 1;

	// Transmit Stop Message
	FEB_CAN_Charger_BMS_Message.max_voltage_dV = 0;
	FEB_CAN_Charger_BMS_Message.max_current_dA = 0;
	FEB_CAN_Charger_BMS_Message.control = 1;
	FEB_CAN_Charger_Transmit(hcan);
}
