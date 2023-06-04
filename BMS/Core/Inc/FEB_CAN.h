#ifndef INC_FEB_CAN_H_
#define INC_FEB_CAN_H_

// ********************************** Includes & External **********************************

#include "stm32f4xx_hal.h"
#include "FEB_CAN_Charger.h"
#include "FEB_CAN_IVT.h"
#include "FEB_CAN_Inverter.h"
#include "FEB_BMS_Shutdown.h"

// ********************************** Functions **********************************

void FEB_CAN_Init();
void FEB_CAN_Filter_Config();
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);

#endif /* INC_FEB_CAN_H_ */
