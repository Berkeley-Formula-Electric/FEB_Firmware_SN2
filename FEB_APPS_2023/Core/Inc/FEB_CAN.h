/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : FEB_CAN.h
  * @brief          : Header for FEB_CAN library.
  *                   This file contains the functions to encapsulate the process
  *                   to initialize a CAN node, transmitting and receiving messages.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include <FEB_NODE_ID.h>

CAN_TxHeaderTypeDef my_TxHeader;
CAN_RxHeaderTypeDef my_RxHeader;
uint8_t TxData[8];
uint8_t RxData[8];
uint32_t TxMailbox;


void FEB_CAN_Filter_Config(CAN_HandleTypeDef *hcan, const AddressIdType* filter_array, uint8_t filter_array_len) {

	for (int i=0; i < filter_array_len; i++) {
		CAN_FilterTypeDef my_can_filter_config;

		my_can_filter_config.FilterActivation = CAN_FILTER_ENABLE;
		my_can_filter_config.FilterBank = i;
		my_can_filter_config.FilterFIFOAssignment = 0;
		my_can_filter_config.FilterIdHigh = filter_array[i] << 9; //Std ID is 11 bits, FilterIDHigh is the higher 16 bits
		my_can_filter_config.FilterIdLow = 0;
		my_can_filter_config.FilterMaskIdHigh = 0x7F0 << 5; //check the first 7 bit (node address)
		my_can_filter_config.FilterMaskIdLow = 0;
		my_can_filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
		my_can_filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
		my_can_filter_config.SlaveStartFilterBank = 27;

		if(HAL_CAN_ConfigFilter(hcan, &my_can_filter_config))
		{
		  Error_Handler();
		}
	}
}

void FEB_CAN_Init(CAN_HandleTypeDef *hcan, uint32_t NODE_ID)
{
	// Setting up transmission header
	my_TxHeader.DLC = 1;
	my_TxHeader.StdId = 0x7FF;
	my_TxHeader.ExtId = 0;
	my_TxHeader.IDE = CAN_ID_STD;
	my_TxHeader.RTR = CAN_RTR_DATA;
	my_TxHeader.TransmitGlobalTime = DISABLE;

	// Assigning the array of sender and numbers of sender according to the node being initialized
	const AddressIdType* filter_array;
	FilterArrayLength filter_num;
	filter_array  = assign_filter_array(NODE_ID);
	filter_num = assign_filter_array_legnth(NODE_ID);

	// Using the assigned array and num to configure filters
	FEB_CAN_Filter_Config(hcan, filter_array, filter_num);

	// Starting the CAN peripheral and enabling the receive interrupt
	if (HAL_CAN_Start(hcan) != HAL_OK) {
	  Error_Handler();
	}
	HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

}

void FEB_CAN_Receive(CAN_HandleTypeDef *hcan) {
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &my_RxHeader, RxData);
	store_msg(&my_RxHeader, RxData);
}

void FEB_CAN_Transmit(CAN_HandleTypeDef *hcan, AddressIdType Msg_ID, void *pData, uint8_t size) {
	// copy the data in to TxData buffer
	memcpy(TxData, pData, size);

	// change the TxHeader based on the message type
	my_TxHeader.StdId = Msg_ID;
	my_TxHeader.DLC = size;

	if (HAL_CAN_AddTxMessage(hcan, &my_TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
	  Error_Handler();
	}
}
