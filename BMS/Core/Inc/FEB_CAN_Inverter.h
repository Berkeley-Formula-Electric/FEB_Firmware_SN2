#ifndef INC_FEB_CAN_INVERTER_H_
#define INC_FEB_CAN_INVERTER_H_

// ********************************** Includes **********************************

#include "stm32f4xx_hal.h"
#include "FEB_BMS_Shutdown.h"
#include "FEB_Math.h"

// ********************************** Inverter IDs **********************************

#define FEB_CAN_INVERTER_TEMPERATURE_1_ID 0x0A0
#define FEB_CAN_INVERTER_TEMPERATURE_3_ID 0x0A2

// ********************************** Temperature Thresholds **********************************

#define FEB_CAN_INVERTER_TEMPERATURE_LIMIT_TORQUE 	55
#define FEB_CAN_INVERTER_TEMPERATURE_MAX 			59

// ********************************** Type Definitions **********************************

typedef struct FEB_CAN_INVERTER_TEMPERATURE_1_MESSAGE_TYPE {
	int16_t phase_a_dC;
	int16_t phase_b_dC;
	int16_t phase_c_dC;
	int16_t gate_driver_board_dC;
} FEB_CAN_INVERTER_TEMPERATURE_1_MESSAGE_TYPE;

typedef struct FEB_CAN_INVERTER_TEMPERATURE_3_MESSAGE_TYPE {
	int16_t motor_dC;
} FEB_CAN_INVERTER_TEMPERATURE_3_MESSAGE_TYPE;

// ********************************** Functions **********************************

uint8_t FEB_CAN_Inverter_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment, uint8_t bank);
void FEB_CAN_Inverter_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t RxData[]);
void FEB_CAN_Inverter_Process(void);

#endif /* INC_FEB_CAN_INVERTER_H_ */
