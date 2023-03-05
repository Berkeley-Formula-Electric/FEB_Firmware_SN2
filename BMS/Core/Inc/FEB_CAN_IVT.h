#ifndef INC_FEB_CAN_IVT_H_
#define INC_FEB_CAN_IVT_H_

// ********************************** Includes **********************************

#include <string.h>

#include "stm32f4xx_hal.h"
#include "main.h"
#include "FEB_LTC6811.h"

// ********************************** CAN Configuration **********************************

// IDs
#define IVT_CURRENT_ID 0x521
#define IVT_VOLTAGE1_ID 0x522
#define IVT_VOLTAGE2_ID 0x523
#define IVT_VOLTAGE3_ID 0x524

// Message Types
#define IVT_CURRENT_TYPE uint32_t
#define IVT_VOLTAGE1_TYPE uint32_t
#define IVT_VOLTAGE2_TYPE uint32_t
#define IVT_VOLTAGE3_TYPE uint32_t

// ********************************** Type Definitions **********************************

typedef struct CAN_FLAG_TYPE {
    uint8_t Ivt_Current;
    uint8_t Ivt_Voltage1;
    uint8_t Ivt_Voltage2;
    uint8_t Ivt_Voltage3;
} CAN_FLAG_TYPE;

typedef struct FEB_CAN_IVT_MESSAGE_TYPE {
    IVT_CURRENT_TYPE Ivt_Current;	// mA
    IVT_VOLTAGE1_TYPE Ivt_Voltage1;	// mV
    IVT_VOLTAGE2_TYPE Ivt_Voltage2;	// mV
    IVT_VOLTAGE3_TYPE Ivt_Voltage3; // mV
} FEB_CAN_IVT_MESSAGE_TYPE;

// ********************************** Functions **********************************

void FEB_CAN_IVT_Init(CAN_HandleTypeDef* hcan);
void FEB_CAN_IVT_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment);
void FEB_CAN_IVT_Store_Msg(CAN_RxHeaderTypeDef *pHeader, uint8_t RxData[]);
void FEB_CAN_IVT_Receive(CAN_HandleTypeDef *hcan, uint32_t CAN_RX_FIFO);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void FEB_CAN_IVT_Process(void);

#endif /* INC_FEB_CAN_H_ */
