// ********************************** Includes & External **********************************

#include "FEB_CAN_IVT.h"

// ********************************** CAN Configuration **********************************

static FEB_CAN_IVT_FLAG_TYPE FEB_CAN_IVT_FLAG;
static FEB_CAN_IVT_MESSAGE_TYPE FEB_CAN_IVT_MESSAGE;
static uint16_t FEB_CAN_IVT_Filter_ID_Arr[] = {
		FEB_CAN_IVT_CURRENT_ID,
		FEB_CAN_IVT_VOLTAGE1_ID,
		FEB_CAN_IVT_VOLTAGE2_ID,
		FEB_CAN_IVT_VOLTAGE3_ID
};

// TODO: Remove UART
extern UART_HandleTypeDef huart2;


// ********************************** Functions **********************************


// ******************** CAN ********************
uint8_t FEB_CAN_IVT_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment, uint8_t bank) {
	for (int i = 0; i < 4; i++, bank++) {
		CAN_FilterTypeDef filter_config;

		filter_config.FilterActivation = CAN_FILTER_ENABLE;
		filter_config.FilterBank = bank;
		filter_config.FilterFIFOAssignment = FIFO_Assignment;
		filter_config.FilterIdHigh = FEB_CAN_IVT_Filter_ID_Arr[i] << 5;
		filter_config.FilterIdLow = 0;
		filter_config.FilterMaskIdHigh = 0x7FF << 5;
		filter_config.FilterMaskIdLow = 0;
		filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
		filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
		filter_config.SlaveStartFilterBank = 27;

		if(HAL_CAN_ConfigFilter(hcan, &filter_config)) {
			Error_Handler();
		}
	}
	return bank;
}

void FEB_CAN_IVT_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t RxData[]) {
    switch(pHeader->StdId) {
    	case FEB_CAN_IVT_CURRENT_ID:
    		memcpy(&FEB_CAN_IVT_MESSAGE.IVT_Current, &RxData[2], 4);
    		FEB_CAN_IVT_FLAG.IVT_Current = 1;
    		break;
    	case FEB_CAN_IVT_VOLTAGE1_ID:
    		memcpy(&FEB_CAN_IVT_MESSAGE.IVT_Voltage1, &RxData[2], 4);
    		FEB_CAN_IVT_FLAG.IVT_Voltage1 = 1;
    		break;
    	case FEB_CAN_IVT_VOLTAGE2_ID:
    		memcpy(&FEB_CAN_IVT_MESSAGE.IVT_Voltage2, &RxData[2], 4);
    		FEB_CAN_IVT_FLAG.IVT_Voltage2 = 1;
    		break;
    	case FEB_CAN_IVT_VOLTAGE3_ID:;
    		uint32_t value = (RxData[2] << 24) + (RxData[3] << 16) + (RxData[4] << 8) + RxData[5];
    		FEB_CAN_IVT_MESSAGE.IVT_Voltage3 = unsignedToSignedLong(value);
    		FEB_CAN_IVT_FLAG.IVT_Voltage3 = 1;
    		break;
    }
}

// ******************** IVT Process Data  ********************

void FEB_CAN_IVT_Process(void) {
	if (FEB_CAN_IVT_FLAG.IVT_Current == 1) {
		FEB_CAN_IVT_FLAG.IVT_Current = 0;
		// Do Something
	}
	if (FEB_CAN_IVT_FLAG.IVT_Voltage1 == 1) {
		FEB_CAN_IVT_FLAG.IVT_Voltage1 = 0;
		// Do something

	}
	if (FEB_CAN_IVT_FLAG.IVT_Voltage2 == 1) {
		FEB_CAN_IVT_FLAG.IVT_Voltage2 = 0;
		// Do Something
	}
	if (FEB_CAN_IVT_FLAG.IVT_Voltage3 == 1) {
		FEB_CAN_IVT_FLAG.IVT_Voltage3 = 0;
		float Ivt_Voltage_V = ((float) FEB_CAN_IVT_MESSAGE.IVT_Voltage3) * 0.001;
		if (Ivt_Voltage_V > FEB_LTC6811_Total_Bank_Voltage() * 0.9) {
			// Broadcast Message
			FEB_BMS_Precharge_Close();
		} else {
			FEB_BMS_Precharge_Open();
		}
	}
}

long unsignedToSignedLong(uint32_t value) {
	long rValue = value & 0x7FFFFFFF;
	rValue -= value & (0b1 << 31);
	return rValue;
}
