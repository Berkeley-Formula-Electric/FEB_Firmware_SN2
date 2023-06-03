#ifndef INC_FEB_CAN_INVERTER_H_
#define INC_FEB_CAN_INVERTER_H_

// ********************************** Includes **********************************

#include "stm32f4xx_hal.h"
#include "FEB_BMS_Shutdown.h"
#include "FEB_Math.h"

// ********************************** Inverter IDs **********************************

#define FEB_CAN_Inverter_Temperature_1_ID 0x0A0
#define FEB_CAN_Inverter_Temperature_3_ID 0x0A2

// ********************************** Temperature Thresholds **********************************

#define FEB_CAN_Inverter_Temperature_Limit_Torque 55
#define FEB_CAN_Inverter_Temperature_Max 59


// ********************************** Type Definitions **********************************

typedef struct FEB_CAN_Inverter_Temperature_1_Message_Type {
	int16_t phase_a;				// 10C
	int16_t phase_b;				// 10C
	int16_t phase_c;				// 10C
	int16_t gate_driver_board;		// 10C
} FEB_CAN_Inverter_Temperature_1_Message_Type;

typedef struct FEB_CAN_Inverter_Temperature_3_Message_Type {
	int16_t motor;					// 10C
} FEB_CAN_Inverter_Temperature_3_Message_Type;

// ********************************** Functions **********************************

uint8_t FEB_CAN_Inverter_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment, uint8_t bank);
void FEB_CAN_Inverter_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t RxData[]);
void FEB_CAN_Inverter_Process(void);

#endif /* INC_FEB_CAN_INVERTER_H_ */
