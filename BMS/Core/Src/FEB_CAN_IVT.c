// ********************************** Includes & External **********************************

#include "FEB_CAN_IVT.h"

// ********************************** CAN Configuration **********************************

static FEB_CAN_IVT_FLAG_TYPE FEB_CAN_IVT_FLAG;
static FEB_CAN_IVT_MESSAGE_TYPE FEB_CAN_IVT_MESSAGE;
static uint16_t FEB_CAN_IVT_Filter_ID_Arr[] = {
		FEB_CAN_IVT_CURRENT_ID,
		FEB_CAN_IVT_VOLTAGE_1_ID,
		FEB_CAN_IVT_VOLTAGE_2_ID,
		FEB_CAN_IVT_VOLTAGE_3_ID
};

// ********************************** Functions **********************************


// ******************** CAN ********************

uint8_t FEB_CAN_IVT_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment, uint8_t bank) {
	const uint8_t num_filters = (uint8_t) FEB_MATH_ARRAY_LENGTH(FEB_CAN_IVT_Filter_ID_Arr);
	for (int i = 0; i < num_filters; i++, bank++) {
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
			FEB_BMS_Shutdown_Initiate("Invalid IVT CAN filter configuration");
		}
	}
	return bank;
}

void FEB_CAN_IVT_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t RxData[]) {
	uint32_t value;
    switch(pHeader->StdId) {
    	case FEB_CAN_IVT_CURRENT_ID:
    		value = (RxData[2] << 24) + (RxData[3] << 16) + (RxData[4] << 8) + RxData[5];
    	    FEB_CAN_IVT_MESSAGE.current_mA = FEB_Math_Uint32_To_Signed_Long(value);
    	    FEB_CAN_IVT_FLAG.current = 1;
    		break;
    	case FEB_CAN_IVT_VOLTAGE_1_ID:
    		value = (RxData[2] << 24) + (RxData[3] << 16) + (RxData[4] << 8) + RxData[5];
    	    FEB_CAN_IVT_MESSAGE.voltage_1_mV = FEB_Math_Uint32_To_Signed_Long(value);
    	    FEB_CAN_IVT_FLAG.voltage_1 = 1;
    		break;
    	case FEB_CAN_IVT_VOLTAGE_2_ID:
    		value = (RxData[2] << 24) + (RxData[3] << 16) + (RxData[4] << 8) + RxData[5];
    	    FEB_CAN_IVT_MESSAGE.voltage_2_mV = FEB_Math_Uint32_To_Signed_Long(value);
    	    FEB_CAN_IVT_FLAG.voltage_2 = 1;
    		break;
    	case FEB_CAN_IVT_VOLTAGE_3_ID:
    		value = (RxData[2] << 24) + (RxData[3] << 16) + (RxData[4] << 8) + RxData[5];
    		FEB_CAN_IVT_MESSAGE.voltage_3_mV = FEB_Math_Uint32_To_Signed_Long(value);
    		FEB_CAN_IVT_FLAG.voltage_3 = 1;
    		break;
    }
}

// ******************** Process Data  ********************

void FEB_CAN_IVT_Process(void) {
	if (FEB_CAN_CHARGER_STATE == 1 || FEB_LTC6811_BALANCE_STATE == 1) {
		return;
	}
	if (FEB_CAN_IVT_FLAG.current == 1) {
		FEB_CAN_IVT_FLAG.current = 0;
		const float current = (float) FEB_CAN_IVT_MESSAGE.current_mA * 0.001;
		if (current > FEB_LTC6811_CELL_MAX_OPERATING_CURRENT) {
			FEB_BMS_Shutdown_Initiate("IVT over current");
		}
	}
	if (FEB_CAN_IVT_FLAG.voltage_1 == 1) {
		FEB_CAN_IVT_FLAG.voltage_1 = 0;
		// Do something

	}
	if (FEB_CAN_IVT_FLAG.voltage_2 == 1) {
		FEB_CAN_IVT_FLAG.voltage_2 = 0;
		// Do Something
	}
	if (FEB_CAN_IVT_FLAG.voltage_3 == 1) {
		FEB_CAN_IVT_FLAG.voltage_3 = 0;
		float voltage = (float) FEB_CAN_IVT_MESSAGE.voltage_3_mV * 0.001;
		if (voltage > FEB_LTC6811_Total_Bank_Voltage() * 0.9) {
		  // comment this out when testing the battery
			FEB_BMS_AIR_Close();
		} else {
			FEB_BMS_AIR_Open();
		}
	}
}
