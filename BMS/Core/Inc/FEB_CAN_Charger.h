#ifndef INC_FEB_CHARGE_H_
#define INC_FEB_CHARGE_H_

// ********************************** Includes **********************************

#include <string.h>
#include "stm32f4xx_hal.h"

#include "FEB_LTC6811.h"
#include "FEB_BMS_Shutdown.h"
#include "FEB_CAN.h"


// ********************************** Charger Configuration **********************************

#define FEB_CAN_CHARGER_START_CHARGE 0 				// 0 (No charge), 1 (Charge)
#define FEB_CAN_CHARGER_MAX_CHARGING_CURRENT 0.1	// 0-12A, 0.1A resolution

// ********************************** CAN Configuration **********************************

#define FEB_CAN_CHARGER_BITS_PER_ID 29
#define FEB_CAN_CHARGER_BMS_ID 0x1806E5F4
#define FEB_CAN_CHARGER_CHARGER_ID 0x18FF50E5

// ********************************** Type Definitions **********************************

typedef struct FEB_CAN_CHARGER_BMS_MESSAGE_TYPE {
	uint16_t max_voltage;	// deci-volts
	uint16_t max_current;	// deci-amps
	uint8_t control;		// control - 0 (start), 1 (stop)
} FEB_CAN_CHARGER_BMS_MESSAGE_TYPE;

typedef struct FEB_CAN_CHAERGER_CHARGER_MESSAGE_TYPE {
	uint16_t operating_voltage;	// deci-volts
	uint16_t operating_current;	// deci-amps
	uint16_t status;
} FEB_CAN_CHAERGER_CHARGER_MESSAGE_TYPE;


// ********************************** Functions **********************************

// Initialize
void FEB_CAN_Charger_Init();

// CAN
uint8_t FEB_CAN_Charger_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_Assignment, uint8_t bank);
void FEB_CAN_Charger_Transmit(CAN_HandleTypeDef* hcan);
void FEB_CAN_Charger_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t RxData[]);

// Process Data
void FEB_CAN_Charger_Validate_Status(uint8_t status);
void FEB_CAN_Charger_Process(CAN_HandleTypeDef* hcan);
void FEB_CAN_Charger_Stop_Charge(CAN_HandleTypeDef* hcan);

#endif /* INC_FEB_CHARGE_H_ */
