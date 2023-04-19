#ifndef INC_FEB_CAN_IVT_H_
#define INC_FEB_CAN_IVT_H_

// ********************************** Includes **********************************

#include <string.h>
#include "stm32f4xx_hal.h"
#include "main.h"

#include "FEB_LTC6811.h"
#include "FEB_BMS_Precharge.h"

// ********************************** CAN Configuration **********************************

// IDs
#define FEB_CAN_IVT_CURRENT_ID 0x521
#define FEB_CAN_IVT_VOLTAGE1_ID 0x522
#define FEB_CAN_IVT_VOLTAGE2_ID 0x523
#define FEB_CAN_IVT_VOLTAGE3_ID 0x524

// Message Types
#define FEB_CAN_IVT_CURRENT_TYPE long
#define FEB_CAN_IVT_VOLTAGE1_TYPE long
#define FEB_CAN_IVT_VOLTAGE2_TYPE long
#define FEB_CAN_IVT_VOLTAGE3_TYPE long

// ********************************** Type Definitions **********************************

typedef struct FEB_CAN_IVT_FLAG_TYPE {
    uint8_t IVT_Current;
    uint8_t IVT_Voltage1;
    uint8_t IVT_Voltage2;
    uint8_t IVT_Voltage3;
} FEB_CAN_IVT_FLAG_TYPE;

typedef struct FEB_CAN_IVT_MESSAGE_TYPE {
	FEB_CAN_IVT_CURRENT_TYPE IVT_Current;	// mA
	FEB_CAN_IVT_VOLTAGE1_TYPE IVT_Voltage1;	// mV
	FEB_CAN_IVT_VOLTAGE2_TYPE IVT_Voltage2;	// mV
	FEB_CAN_IVT_VOLTAGE3_TYPE IVT_Voltage3; // mV
} FEB_CAN_IVT_MESSAGE_TYPE;

// ********************************** Functions **********************************

uint8_t FEB_CAN_IVT_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment, uint8_t bank);
void FEB_CAN_IVT_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t RxData[]);
void FEB_CAN_IVT_Process(void);
long unsignedToSignedLong(uint32_t value);

#endif /* INC_FEB_CAN_H_ */
