// ********************************** Includes & External **********************************

#include "FEB_CAN_Inverter.h"

extern CAN_HandleTypeDef hcan1;

// ********************************** CAN Configuration **********************************

static FEB_CAN_Inverter_Temperature_1_Message_Type FEB_CAN_Inverter_Temperature_1_Message;
static FEB_CAN_Inverter_Temperature_3_Message_Type FEB_CAN_Inverter_Temperature_3_Message;

static uint16_t FEB_CAN_Inverter_Filter_ID_Arr[] = {
		FEB_CAN_Inverter_Temperature_1_ID,
		FEB_CAN_Inverter_Temperature_3_ID
};

int16_t* FEB_CAN_Inverter_Temperatures[] = {
	&FEB_CAN_Inverter_Temperature_1_Message.phase_a,
	&FEB_CAN_Inverter_Temperature_1_Message.phase_b,
	&FEB_CAN_Inverter_Temperature_1_Message.phase_c,
	&FEB_CAN_Inverter_Temperature_1_Message.gate_driver_board,
	&FEB_CAN_Inverter_Temperature_3_Message.motor
};

// ********************************** Functions **********************************

uint8_t FEB_CAN_Inverter_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment, uint8_t bank) {
	for (int i = 0; i < 2; i++, bank++) {
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
		case FEB_CAN_Inverter_Temperature_1_ID:
			FEB_CAN_Inverter_Temperature_1_Message.phase_a = unsignedToSigned16((RxData[0] << 8) + RxData[1]);
			FEB_CAN_Inverter_Temperature_1_Message.phase_b = unsignedToSigned16((RxData[2] << 8) + RxData[3]);
			FEB_CAN_Inverter_Temperature_1_Message.phase_c = unsignedToSigned16((RxData[4] << 8) + RxData[5]);
			FEB_CAN_Inverter_Temperature_1_Message.gate_driver_board = unsignedToSigned16((RxData[6] << 8) + RxData[7]);
			break;
		case FEB_CAN_Inverter_Temperature_3_ID:
			FEB_CAN_Inverter_Temperature_3_Message.motor = unsignedToSigned16((RxData[4] << 8) + RxData[5]);
			break;
	}
}

void FEB_CAN_Inverter_Process(void) {
	for (uint8_t i = 0; i < 5; i++) {
		if (*FEB_CAN_Inverter_Temperatures[i] > FEB_CAN_Inverter_Temperature_Max) {
			FEB_BMS_Shutdown_Initiate("Inverter over temperature");
		} else if (*FEB_CAN_Inverter_Temperatures[i] > FEB_CAN_Inverter_Temperature_Limit_Torque) {
			// TODO: Limit torque
		}
	}
}

