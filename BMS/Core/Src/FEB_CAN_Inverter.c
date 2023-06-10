// ********************************** Includes & External **********************************

#include "FEB_CAN_Inverter.h"

extern CAN_HandleTypeDef hcan1;

// ********************************** CAN Configuration **********************************

static FEB_CAN_INVERTER_TEMPERATURE_1_MESSAGE_TYPE FEB_CAN_Inverter_Temperature_1_Message;
static FEB_CAN_INVERTER_TEMPERATURE_3_MESSAGE_TYPE FEB_CAN_Inverter_Temperature_3_Message;

static uint16_t FEB_CAN_Inverter_Filter_ID_Arr[] = {
		FEB_CAN_INVERTER_TEMPERATURE_1_ID,
		FEB_CAN_INVERTER_TEMPERATURE_3_ID
};

int16_t* FEB_CAN_Inverter_Temperatures_dC[] = {
	&FEB_CAN_Inverter_Temperature_1_Message.phase_a_dC,
	&FEB_CAN_Inverter_Temperature_1_Message.phase_b_dC,
	&FEB_CAN_Inverter_Temperature_1_Message.phase_c_dC,
	&FEB_CAN_Inverter_Temperature_1_Message.gate_driver_board_dC,
	&FEB_CAN_Inverter_Temperature_3_Message.motor_dC
};

// ********************************** Functions **********************************

// ******************** CAN ********************

uint8_t FEB_CAN_Inverter_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment, uint8_t bank) {
	uint8_t num_filters = FEB_MATH_ARRAY_LENGTH(FEB_CAN_Inverter_Filter_ID_Arr);
	for (int i = 0; i < num_filters; i++, bank++) {
		CAN_FilterTypeDef filter_config;

		filter_config.FilterActivation = CAN_FILTER_ENABLE;
		filter_config.FilterBank = bank;
		filter_config.FilterFIFOAssignment = FIFO_Assignment;
		filter_config.FilterIdHigh = FEB_CAN_Inverter_Filter_ID_Arr[i] << 5;
		filter_config.FilterIdLow = 0;
		filter_config.FilterMaskIdHigh = 0x7FF << 5;
		filter_config.FilterMaskIdLow = 0;
		filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
		filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
		filter_config.SlaveStartFilterBank = 27;

		if (HAL_CAN_ConfigFilter(hcan, &filter_config)) {
			FEB_BMS_Shutdown_Initiate("Invalid inverter CAN filter configuration");
		}
	}
	return bank;
}

void FEB_CAN_Inverter_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t RxData[]) {
	switch (pHeader->StdId) {
		case FEB_CAN_INVERTER_TEMPERATURE_1_ID:
			FEB_CAN_Inverter_Temperature_1_Message.phase_a_dC = FEB_Math_Uint16_To_Int16((RxData[0] << 8) + RxData[1]);
			FEB_CAN_Inverter_Temperature_1_Message.phase_b_dC = FEB_Math_Uint16_To_Int16((RxData[2] << 8) + RxData[3]);
			FEB_CAN_Inverter_Temperature_1_Message.phase_c_dC = FEB_Math_Uint16_To_Int16((RxData[4] << 8) + RxData[5]);
			FEB_CAN_Inverter_Temperature_1_Message.gate_driver_board_dC = FEB_Math_Uint16_To_Int16((RxData[6] << 8) + RxData[7]);
			break;
		case FEB_CAN_INVERTER_TEMPERATURE_3_ID:
			FEB_CAN_Inverter_Temperature_3_Message.motor_dC = FEB_Math_Uint16_To_Int16((RxData[4] << 8) + RxData[5]);
			break;
	}
}

// ******************** Process Data ********************

void FEB_CAN_Inverter_Process(void) {
	uint8_t num_temperatures = FEB_MATH_ARRAY_LENGTH(FEB_CAN_Inverter_Temperatures_dC);
	for (uint8_t i = 0; i < num_temperatures; i++) {
		float temperature = *FEB_CAN_Inverter_Temperatures_dC[i] * 0.1;
		if (temperature > FEB_CAN_INVERTER_TEMPERATURE_MAX) {
			FEB_BMS_Shutdown_Initiate("Inverter over temperature");
		} else if (temperature > FEB_CAN_INVERTER_TEMPERATURE_LIMIT_TORQUE) {
			// TODO: Limit torque
		}
	}
}

