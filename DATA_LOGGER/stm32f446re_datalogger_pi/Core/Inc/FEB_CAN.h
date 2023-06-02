#ifndef INC_FEB_CAN_H_
#define INC_FEB_CAN_H_

#include "FEB_CAN_NODE.h"

CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
uint8_t TxData[8];
uint8_t RxData[8];
uint32_t TxMailbox;
uint8_t CAN_Flag = 0;

void FEB_CAN_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment) {
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

	if(HAL_CAN_ConfigFilter(hcan, &filter_config))
	{
	  Error_Handler();
	}
}

void FEB_CAN_Init(CAN_HandleTypeDef* hcan) {
	// Select Rx FIFO
	uint8_t FIFO_Assignment;
	uint32_t FIFO_Interrupt;
	if (hcan->Instance == CAN1) {
		FIFO_Assignment = CAN_RX_FIFO0;
		FIFO_Interrupt = CAN_IT_RX_FIFO0_MSG_PENDING;
	} else if (hcan->Instance == CAN2) {
		FIFO_Assignment = CAN_RX_FIFO1;
		FIFO_Interrupt = CAN_IT_RX_FIFO1_MSG_PENDING;
	}

	// Initialize transmission header
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.TransmitGlobalTime = DISABLE;

	// Initialize CAN filters
	FEB_CAN_Filter_Config(hcan, FIFO_Assignment);

	// Start CAN peripheral
	if (HAL_CAN_Start(hcan) != HAL_OK) {
	  Error_Handler();
	}

	// Activate receive interrupt
	HAL_CAN_ActivateNotification(hcan, FIFO_Interrupt);

}

void FEB_CAN_Receive(CAN_HandleTypeDef *hcan, uint32_t CAN_RX_FIFO) {
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO, &RxHeader, RxData) != HAL_OK) {
		Error_Handler();
	}
	store_msg(&RxHeader, RxData);
}

void FEB_CAN_Transmit(CAN_HandleTypeDef* hcan, AddressIdType Msg_ID, void* pData, uint8_t size) {
	// Copy data to Tx buffer
	memcpy(TxData, pData, size);

	// Update Tx header
	TxHeader.StdId = Msg_ID;
	TxHeader.DLC = size;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
	  Error_Handler();
	}
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	FEB_CAN_Receive(hcan, CAN_RX_FIFO0);
	CAN_Flag = 1;
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	FEB_CAN_Receive(hcan, CAN_RX_FIFO1);
	CAN_Flag = 1;
}

#endif /* INC_FEB_CAN_H_ */
