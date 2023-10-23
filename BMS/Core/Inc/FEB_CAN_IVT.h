#ifndef INC_FEB_CAN_IVT_H_
#define INC_FEB_CAN_IVT_H_

// ********************************** Includes **********************************

#include <FEB_BMS_AIR.h>
#include "string.h"

#include "stm32f4xx_hal.h"
#include "FEB_LTC6811.h"
#include "FEB_BMS_Shutdown.h"
#include "FEB_Math.h"
#include "FEB_BMS_AIR.h"

// ********************************** CAN Configuration **********************************

// IDs
#define FEB_CAN_IVT_CURRENT_ID 		0x521
#define FEB_CAN_IVT_VOLTAGE_1_ID 	0x522
#define FEB_CAN_IVT_VOLTAGE_2_ID 	0x523
#define FEB_CAN_IVT_VOLTAGE_3_ID 	0x524

// ********************************** Type Definitions **********************************

typedef struct FEB_CAN_IVT_FLAG_TYPE {
    uint8_t current;
    uint8_t voltage_1;
    uint8_t voltage_2;
    uint8_t voltage_3;
} FEB_CAN_IVT_FLAG_TYPE;

typedef struct FEB_CAN_IVT_MESSAGE_TYPE {
	long current_mA;
	long voltage_1_mV;
	long voltage_2_mV;
	long voltage_3_mV;
} FEB_CAN_IVT_MESSAGE_TYPE;

// ********************************** Functions **********************************
uint8_t FEB_CAN_IVT_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment, uint8_t bank);
void FEB_CAN_IVT_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t RxData[]);
void FEB_CAN_IVT_Process(void);
void FEB_CAN_IVT_UART_Transmit(void);
//long unsignedToSignedLong(uint32_t value);

#endif /* INC_FEB_CAN_H_ */
