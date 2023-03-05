// ********************************** Includes & External **********************************

#include "FEB_CAN_IVT.h"

// ********************************** CAN Configuration **********************************

static CAN_FLAG_TYPE FEB_CAN_IVT_FLAG;
static FEB_CAN_IVT_MESSAGE_TYPE IVT_MESSAGE;

static CAN_TxHeaderTypeDef FEB_CAN_IVT_TxHeader;
static CAN_RxHeaderTypeDef FEB_CAN_IVT_RxHeader;
// static uint8_t FEB_CAN_IVT_TxData[8];
static uint8_t FEB_CAN_IVT_RxData[8];
// static uint32_t FEB_CAN_IVT_TxMailbox;
static uint8_t FEB_CAN_IVT_Flag = 0;

// ********************************** Functions **********************************


// ******************** CAN ********************

void FEB_CAN_IVT_Init(CAN_HandleTypeDef* hcan) {
	// Select Rx FIFO
	uint8_t FIFO_Assignment = CAN_RX_FIFO0;
	uint32_t FIFO_Interrupt = CAN_IT_RX_FIFO0_MSG_PENDING;

	// Initialize transmission header
	FEB_CAN_IVT_TxHeader.IDE = CAN_ID_STD;
	FEB_CAN_IVT_TxHeader.RTR = CAN_RTR_DATA;
	FEB_CAN_IVT_TxHeader.TransmitGlobalTime = DISABLE;

	// Initialize CAN filters
	FEB_CAN_IVT_Filter_Config(hcan, FIFO_Assignment);

	// Start CAN peripheral
	if (HAL_CAN_Start(hcan) != HAL_OK) {
	  Error_Handler();
	}

	// Activate receive interrupt
	HAL_CAN_ActivateNotification(hcan, FIFO_Interrupt);
}

void FEB_CAN_IVT_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment) {

	CAN_FilterTypeDef filter_config;

	filter_config.FilterActivation = CAN_FILTER_ENABLE;
	filter_config.FilterBank = 0;
	filter_config.FilterFIFOAssignment = FIFO_Assignment;
	filter_config.FilterIdHigh = 0;
	filter_config.FilterIdLow = 0;
	filter_config.FilterMaskIdHigh = 0;
	filter_config.FilterMaskIdLow = 0;
	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	filter_config.SlaveStartFilterBank = 27;

	if(HAL_CAN_ConfigFilter(hcan, &filter_config)) {
		Error_Handler();
	}
}

void FEB_CAN_IVT_Store_Msg(CAN_RxHeaderTypeDef *pHeader, uint8_t RxData[]) {
    switch(pHeader->StdId) {
    	case IVT_CURRENT_ID:
    		memcpy(&IVT_MESSAGE.Ivt_Current, &RxData[2], 4);
    		FEB_CAN_IVT_FLAG.Ivt_Current = 1;
    		break;
    	case IVT_VOLTAGE1_ID:
    		memcpy(&IVT_MESSAGE.Ivt_Voltage1, &RxData[2], 4);
    		FEB_CAN_IVT_FLAG.Ivt_Voltage1 = 1;
    		break;
    	case IVT_VOLTAGE2_ID:
    		memcpy(&IVT_MESSAGE.Ivt_Voltage2, &RxData[2], 4);
    		FEB_CAN_IVT_FLAG.Ivt_Voltage2 = 1;
    		break;
    	case IVT_VOLTAGE3_ID:
    		memcpy(&IVT_MESSAGE.Ivt_Voltage3, &RxData[2], 4);
    		FEB_CAN_IVT_FLAG.Ivt_Voltage3 = 1;
    		break;
    }
}

void FEB_CAN_IVT_Receive(CAN_HandleTypeDef *hcan, uint32_t CAN_RX_FIFO) {
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO, &FEB_CAN_IVT_RxHeader, FEB_CAN_IVT_RxData) != HAL_OK) {
		Error_Handler();
	}
	FEB_CAN_IVT_Store_Msg(&FEB_CAN_IVT_RxHeader, FEB_CAN_IVT_RxData);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	FEB_CAN_IVT_Receive(hcan, CAN_RX_FIFO0);
	FEB_CAN_IVT_Flag = 1;
}

// ******************** IVT Process Data  ********************

void FEB_CAN_IVT_Process(void) {
	if (FEB_CAN_IVT_FLAG.Ivt_Current == 1) {
		FEB_CAN_IVT_FLAG.Ivt_Current = 0;
		// Do Something
	}
	if (FEB_CAN_IVT_FLAG.Ivt_Voltage1 == 1) {
		FEB_CAN_IVT_FLAG.Ivt_Voltage1 = 0;

		float Ivt_Voltage = IVT_MESSAGE.Ivt_Voltage1 * 0.001;

		if (Ivt_Voltage > FEB_LTC6811_Total_Bank_Voltage() * 0.9) {
			// OPEN AIRS
		}

	}
	if (FEB_CAN_IVT_FLAG.Ivt_Voltage2 == 1) {
		FEB_CAN_IVT_FLAG.Ivt_Voltage2 = 0;
		// Do Something
	}
	if (FEB_CAN_IVT_FLAG.Ivt_Voltage3 == 1) {
		FEB_CAN_IVT_FLAG.Ivt_Voltage3 = 0;
		// Do Something
	}
}
