/*
 * FEB_charge.h
 *
 *  Created on: Nov 20, 2022
 *      Author: aman
 */

#ifndef INC_FEB_CHARGE_H_
#define INC_FEB_CHARGE_H_
#endif /* INC_FEB_CHARGE_H_ */

/* USER CODE BEGIN Header */

#define BITS_PER_ID 29

#define BMS_ID 0x1806E5F4
#define CHARGER_ID 0x18FF50E5

// data structs
// max_voltage [deci-volt]
// max_current [deci-amp]
// control - 0 (start), 1 (stop)
typedef struct BMS_MESSAGE_TYPE {
	uint16_t max_voltage;
	uint16_t max_current;
	uint8_t control;
} BMS_MESSAGE_TYPE;
BMS_MESSAGE_TYPE BMS_MESSAGE;

// data structs
// operating_voltage [deci-volt]
// operating_current [deci-amp]
typedef struct CHARGER_MESSAGE_TYPE {
	uint16_t operating_voltage;
	uint16_t operating_current;
	uint16_t status;
} CHARGER_MESSAGE_TYPE;
CHARGER_MESSAGE_TYPE CHARGER_MESSAGE;

// CAN handlers
CAN_TxHeaderTypeDef my_TxHeader;
CAN_RxHeaderTypeDef my_RxHeader;
uint8_t TxData[8];
uint8_t RxData[8];
uint32_t TxMailbox;

uint8_t CAN_Rx_flag = 0;

// declare functions
void store_charger_msg(uint8_t RxData[]);


void FEB_set_rx_flag() {
	CAN_Rx_flag = 1;
}

void FEB_reset_rx_flag() {
	CAN_Rx_flag = 0;
}

void FEB_CAN_Filter_Config(CAN_HandleTypeDef *hcan) {
	CAN_FilterTypeDef my_can_filter_config;

	my_can_filter_config.FilterActivation = CAN_FILTER_ENABLE;
	my_can_filter_config.FilterBank = 0;
	my_can_filter_config.FilterFIFOAssignment = 0;
	my_can_filter_config.FilterIdHigh = CHARGER_ID >> (BITS_PER_ID - 16);	// set first 16 bits
	my_can_filter_config.FilterIdLow = CHARGER_ID & 0x1FFF;					// set last 13 bits
	my_can_filter_config.FilterMaskIdHigh = 0xFFFF;							// mask first 16 bits
	my_can_filter_config.FilterMaskIdLow = 0x1FFF;							// mask last 13 bits
	my_can_filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	my_can_filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	my_can_filter_config.SlaveStartFilterBank = 27;

	if(HAL_CAN_ConfigFilter(hcan, &my_can_filter_config)) {
	  Error_Handler();
	}
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &my_RxHeader, RxData);
	store_charger_msg(RxData);
	FEB_set_rx_flag();
}


void store_charger_msg(uint8_t RxData[]) {
	uint16_t operating_voltage = (RxData[0] << 8) + RxData[1];
	uint16_t operating_current = (RxData[2] << 8) + RxData[3];
	uint8_t control = RxData[4];

	memcpy(&(CHARGER_MESSAGE.operating_voltage), &operating_voltage, 2);
	memcpy(&(CHARGER_MESSAGE.operating_current), &operating_current, 2);
	memcpy(&(CHARGER_MESSAGE.status), &control, 1);
}

void FEB_CAN_Init(CAN_HandleTypeDef *hcan)
{
	// Setting up transmission header
	my_TxHeader.DLC = 8;
	my_TxHeader.ExtId = BMS_ID;
	my_TxHeader.IDE = CAN_ID_EXT;
	my_TxHeader.RTR = CAN_RTR_DATA;
	my_TxHeader.TransmitGlobalTime = DISABLE;

	// Using the assigned array and num to configure filters
	FEB_CAN_Filter_Config(hcan);

	// Starting the CAN peripheral and enabling the receive interrupt
	if (HAL_CAN_Start(hcan) != HAL_OK) {
	  Error_Handler();
	}
	HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void FEB_CAN_Transmit(CAN_HandleTypeDef *hcan) {
	// copy the data in to TxData buffer
	TxData[0] = BMS_MESSAGE.max_voltage >> 8;
	TxData[1] = BMS_MESSAGE.max_voltage & 0xFF;
	TxData[2] = BMS_MESSAGE.max_current >> 8;
	TxData[3] = BMS_MESSAGE.max_current & 0xFF;
	TxData[4] = BMS_MESSAGE.control;
	TxData[5] = 0;
	TxData[6] = 0;
	TxData[7] = 0;

	if (HAL_CAN_AddTxMessage(hcan, &my_TxHeader, TxData, &TxMailbox) != HAL_OK)
	{
	  Error_Handler();
	}
}


