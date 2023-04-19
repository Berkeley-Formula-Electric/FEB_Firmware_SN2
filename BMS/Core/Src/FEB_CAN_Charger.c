// ********************************** Includes & External **********************************

#include "FEB_CAN_Charger.h"

extern UART_HandleTypeDef huart2;

// ********************************** CAN Configuration **********************************

static FEB_CAN_CHARGER_BMS_MESSAGE_TYPE FEB_CAN_Charger_BMS_Message;
static FEB_CAN_CHAERGER_CHARGER_MESSAGE_TYPE FEB_CAN_Charger_Charger_Message;

// CAN handlers
static CAN_TxHeaderTypeDef FEB_CAN_Charger_TxHeader;
static CAN_RxHeaderTypeDef FEB_CAN_Charger_RxHeader;
static uint8_t FEB_CAN_Charger_TxData[8];
static uint8_t FEB_CAN_Charger_RxData[8];
static uint32_t FEB_CAN_Charger_TxMailbox;

static uint8_t FEB_CAN_Charger_Rx_Flag = 0;

// ********************************** CAN TEST **********************************
uint8_t charger_msg = 0;

// ********************************** Charger Configuration **********************************

static uint8_t FEB_CAN_Charger_State_Bool = 0;					// 0 (actively charging), 1 (finished charging)
static uint8_t FEB_CAN_Charger_Communication_State_Bool = 0;	// Monitor 2 consecutive communication state errors, 0 (no error), 1 (error)

// ********************************** Functions **********************************

void FEB_CAN_Charger_Set_Rx_Flag() {
	FEB_CAN_Charger_Rx_Flag = 1;
	charger_msg = 1;
}

void FEB_CAN_Charger_Reset_Rx_Flag() {
	FEB_CAN_Charger_Rx_Flag = 0;
}

// ******************** Startup ********************

void FEB_CAN_Charger_Init(CAN_HandleTypeDef* hcan) {
	if (FEB_CAN_CHARGER_CHARGE_BOOL == 0) {
		FEB_LTC6811_Clear_Balance_Cells();
		return;
	}

	FEB_CAN_Charger_CAN_Init(hcan);

	// Initialize charger settings
	FEB_CAN_Charger_BMS_Message.max_voltage = (uint16_t) (MAX_VOLTAGE * CELLS_PER_BANK * NUM_BANKS * 10);
	FEB_CAN_Charger_BMS_Message.max_current = (uint16_t) (MAX_CURRENT * 10);
	FEB_CAN_Charger_BMS_Message.control = 0;

	// Set Charger
	if (FEB_CAN_CHARGER_CHARGE_BOOL) {
		FEB_CAN_Charger_State_Bool = 1;
	} else {
		FEB_CAN_Charger_State_Bool = 0;
	}
}

// ******************** CAN ********************

void FEB_CAN_Charger_CAN_Init(CAN_HandleTypeDef* hcan) {
	// Select Rx FIFO
	uint8_t FIFO_Assignment = CAN_RX_FIFO0;
	uint32_t FIFO_Interrupt = CAN_IT_RX_FIFO0_MSG_PENDING;

	// Initialize transmission header
	FEB_CAN_Charger_TxHeader.DLC = 8;
	FEB_CAN_Charger_TxHeader.ExtId = FEB_CAN_CHARGER_BMS_ID;
	FEB_CAN_Charger_TxHeader.IDE = CAN_ID_EXT;
	FEB_CAN_Charger_TxHeader.RTR = CAN_RTR_DATA;
	FEB_CAN_Charger_TxHeader.TransmitGlobalTime = DISABLE;

	// Initialize CAN filters
	FEB_CAN_Charger_Filter_Config(hcan, FIFO_Assignment);

	// Start CAN peripheral
	if (HAL_CAN_Start(hcan) != HAL_OK) {
	  Error_Handler();
	}

	// Activate receive interrupt
	HAL_CAN_ActivateNotification(hcan, FIFO_Interrupt);
}

void FEB_CAN_Charger_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment) {
	CAN_FilterTypeDef my_can_filter_config;

	my_can_filter_config.FilterActivation = CAN_FILTER_ENABLE;
	my_can_filter_config.FilterBank = 0;
	my_can_filter_config.FilterFIFOAssignment = FIFO_Assignment;
	my_can_filter_config.FilterIdHigh = FEB_CAN_CHARGER_CHARGER_ID >> (FEB_CAN_CHARGER_BITS_PER_ID - 16); 			// First 16 bits
	my_can_filter_config.FilterIdLow = (FEB_CAN_CHARGER_CHARGER_ID & 0x1FFF) << (32 - FEB_CAN_CHARGER_BITS_PER_ID);	// Last 13 bits
	my_can_filter_config.FilterMaskIdHigh = 0xFFFF; // mask first 16 bits
	my_can_filter_config.FilterMaskIdLow = 0xFFF8;	// mask last 13 bits
	my_can_filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	my_can_filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	my_can_filter_config.SlaveStartFilterBank = 27;

	if(HAL_CAN_ConfigFilter(hcan, &my_can_filter_config)) {
	  Error_Handler();
	}
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef* hcan) {
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &FEB_CAN_Charger_RxHeader, FEB_CAN_Charger_RxData);
	FEB_CAN_Charger_Store_Msg(FEB_CAN_Charger_RxData, hcan);
	FEB_CAN_Charger_Set_Rx_Flag();

	char str[128];
	sprintf(str, "Message Rx! %li, %li, %li\n", FEB_CAN_Charger_RxHeader.ExtId, FEB_CAN_Charger_RxHeader.StdId, FEB_CAN_Charger_RxHeader.DLC);
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
}


void FEB_CAN_Charger_Store_Msg(uint8_t RxData[], CAN_HandleTypeDef* hcan) {
	uint16_t operating_voltage = (RxData[0] << 8) + RxData[1];
	uint16_t operating_current = (RxData[2] << 8) + RxData[3];
	uint8_t status = RxData[4];

	FEB_CAN_Charger_Validate_Status(status, hcan);

	// Store data
	memcpy(&(FEB_CAN_Charger_Charger_Message.operating_voltage), &operating_voltage, 2);
	memcpy(&(FEB_CAN_Charger_Charger_Message.operating_current), &operating_current, 2);
	memcpy(&(FEB_CAN_Charger_Charger_Message.status), &status, 1);
}

void FEB_CAN_Charger_Transmit(CAN_HandleTypeDef* hcan) {
	// Copy data to Tx buffer
	FEB_CAN_Charger_TxData[0] = FEB_CAN_Charger_BMS_Message.max_voltage >> 8;
	FEB_CAN_Charger_TxData[1] = FEB_CAN_Charger_BMS_Message.max_voltage & 0xFF;
	FEB_CAN_Charger_TxData[2] = FEB_CAN_Charger_BMS_Message.max_current >> 8;
	FEB_CAN_Charger_TxData[3] = FEB_CAN_Charger_BMS_Message.max_current & 0xFF;
	FEB_CAN_Charger_TxData[4] = FEB_CAN_Charger_BMS_Message.control;
	FEB_CAN_Charger_TxData[5] = 0;
	FEB_CAN_Charger_TxData[6] = 0;
	FEB_CAN_Charger_TxData[7] = 0;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(hcan, &FEB_CAN_Charger_TxHeader, FEB_CAN_Charger_TxData, &FEB_CAN_Charger_TxMailbox) != HAL_OK) {
	  Error_Handler();
	}
}

// ******************** Interface  ********************

void FEB_CAN_Charger_Process(CAN_HandleTypeDef* hcan) {
	if (FEB_CAN_CHARGER_CHARGE_BOOL == 0) {
		return;
	}

	if (FEB_CAN_Charger_Rx_Flag == 1) {
		FEB_CAN_Charger_Reset_Rx_Flag();

		if (FEB_CAN_Charger_Charger_Message.operating_voltage * 0.1 > MAX_VOLTAGE ||
			FEB_CAN_Charger_Charger_Message.operating_current * 0.1 > MAX_CURRENT) {
			FEB_CAN_Charger_Stop_Charge(hcan);
			FEB_BMS_Shutdown_Initiate();
		}
	}

	// Check if stopped charging
	if (FEB_CAN_Charger_State_Bool == 0) {
		return;
	}

	// Check all cells are charged
	if (FEB_LTC6811_Cells_Charged() == 1) {
		FEB_CAN_Charger_Stop_Charge(hcan);
	} else {
		// Continue charging
		FEB_CAN_Charger_Transmit(hcan);
		FEB_LTC6811_Balance_Cells();
	}
}

void FEB_CAN_Charger_Stop_Charge(CAN_HandleTypeDef* hcan) {
	FEB_LTC6811_Clear_Balance_Cells();
	FEB_CAN_Charger_State_Bool = 0;

	// Transmit Stop Message
	FEB_CAN_Charger_BMS_Message.max_voltage = 0;
	FEB_CAN_Charger_BMS_Message.max_current = 0;
	FEB_CAN_Charger_BMS_Message.control = 1;
	FEB_CAN_Charger_Transmit(hcan);
}

void FEB_CAN_Charger_Validate_Status(uint8_t status, CAN_HandleTypeDef* hcan) {
	// Failure bits, 0 (no error), 1 (error)
	uint8_t hardware_failure 			= (status >> 7) & 0b1;
	uint8_t temperature_failure 		= (status >> 6) & 0b1;
	uint8_t input_voltage_failure 		= (status >> 5) & 0b1;
	uint8_t starting_state_failure 		= (status >> 4) & 0b1;
	uint8_t communication_state_failure = (status >> 3) & 0b1;

	if (hardware_failure == 1 			||
		temperature_failure == 1 		||
		input_voltage_failure == 1		||
		starting_state_failure == 1		) {
		FEB_BMS_Shutdown_Initiate();
	}

	// Trigger shutdown circuit for 2 consecutive communication state errors
	if (FEB_CAN_Charger_Communication_State_Bool == 1) {
		if (communication_state_failure == 1) {
			FEB_BMS_Shutdown_Initiate();
		} else {
			FEB_CAN_Charger_Communication_State_Bool = 0;
		}
	} else if (FEB_CAN_Charger_Communication_State_Bool == 0 && communication_state_failure == 1) {
		FEB_CAN_Charger_Communication_State_Bool = 1;
	}
}
