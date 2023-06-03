#ifndef INC_FEB_FAN_H_
#define INC_FEB_FAN_H_

// ********************************** Includes & External **********************************

#include "stm32f4xx_hal.h"
#include "FEB_Timer.h"
#include "FEB_BMS_Shutdown.h"
#include "FEB_CAN_Charger.h"

// ********************************** Constants **********************************

#define FEB_Fan_ADC_Value_Limit 100		// 0 - 4095

// ********************************** Functions **********************************

void FEB_Fan_Init(void);

// PWM
void FEB_Fan_PWM_Start(void);
void FEB_Fan_Init_Speed_Set(void);
void FEB_Fan_1_Speed_Set(uint8_t speed);
void FEB_Fan_2_Speed_Set(uint8_t speed);
void FEB_Fan_3_Speed_Set(uint8_t speed);
void FEB_Fan_4_Speed_Set(uint8_t speed);

// Shift Register Control
void FEB_Fan_Reset_Shift_Register(void);
void FEB_Fan_Serial_High(void);
void FEB_Fan_Serial_Low(void);
void FEB_Fan_Clock_High(void);
void FEB_Fan_Clock_Low(void);
void FEB_Fan_Clock_Pulse(void);

// Tachometer
void FEB_Fan_Process(void);
void FEB_Fan_Set_Tachometer(uint8_t value);
uint32_t FEB_Fan_Read_Tachometer(uint8_t multiplex);
void FEB_Fan_Validate_Speed(uint32_t adc_value, uint8_t fan_bank);

#endif /* INC_FEB_FAN_H_ */
