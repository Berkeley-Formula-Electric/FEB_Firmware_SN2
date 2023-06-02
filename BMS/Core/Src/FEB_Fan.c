// ********************************** Includes & External **********************************

#include "FEB_Fan.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

// ********************************** Fan Configuration **********************************

static uint8_t FEB_Fan_1_Speed = 0;		// 0-255
static uint8_t FEB_Fan_2_Speed = 0;		// 0-255
static uint8_t FEB_Fan_3_Speed = 0;		// 0-255
static uint8_t FEB_Fan_4_Speed = 0;		// 0-255

// ********************************** Initialize **********************************

void FEB_Fan_Init(void) {
	FEB_Fan_PWM_Start();
	FEB_Fan_Init_Speed_Set();
	FEB_Fan_Reset_Shift_Register();
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start(&hadc2);
}

// ********************************** PWM **********************************

void FEB_Fan_PWM_Start(void) {
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}

void FEB_Fan_Init_Speed_Set(void) {
	FEB_Fan_1_Speed_Set(255);
	FEB_Fan_2_Speed_Set(255);
	FEB_Fan_3_Speed_Set(255);
	FEB_Fan_4_Speed_Set(255);
}

void FEB_Fan_1_Speed_Set(uint8_t speed) {
	FEB_Fan_1_Speed = speed;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, FEB_Fan_1_Speed);
}

void FEB_Fan_2_Speed_Set(uint8_t speed) {
	FEB_Fan_2_Speed = speed;
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, FEB_Fan_2_Speed);
}

void FEB_Fan_3_Speed_Set(uint8_t speed) {
	FEB_Fan_3_Speed = speed;
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, FEB_Fan_3_Speed);
}

void FEB_Fan_4_Speed_Set(uint8_t speed) {
	FEB_Fan_4_Speed = speed;
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, FEB_Fan_4_Speed);
}

// ********************************** Shift Register Control **********************************

void FEB_Fan_Reset_Shift_Register(void) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
	FEB_Timer_Delay_Micro(10);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
}

void FEB_Fan_Serial_High(void) {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
}

void FEB_Fan_Serial_Low(void) {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
}

void FEB_Fan_Clock_High(void) {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
}

void FEB_Fan_Clock_Low(void) {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
}

void FEB_Fan_Clock_Pulse(void) {
	FEB_Fan_Clock_High();
	FEB_Timer_Delay_Micro(10);
	FEB_Fan_Clock_Low();
	FEB_Timer_Delay_Micro(10);
}

// ********************************** Tachometer **********************************

void FEB_Fan_Process(void) {
	for (uint8_t tach = 0; tach < 12; tach++) {
		uint8_t multiplex = tach < 6 ? 0 : 1;
		uint8_t tach_pin = tach < 6 ? tach : (tach - 6) << 3;
		uint8_t fan_bank = (tach / 3) + 1;
		FEB_Fan_Set_Tachometer(tach_pin);
		uint32_t adc_value = FEB_Fan_Read_Tachometer(multiplex);
		FEB_Fan_Validate_Speed(adc_value, fan_bank);
	}
}

void FEB_Fan_Set_Tachometer(uint8_t value) {
	for (uint8_t i = 0; i < 8; i++) {
		// Set multiplex
		if (((value << i) & 0b1) == 1) {
			FEB_Fan_Serial_High();
		} else {
			FEB_Fan_Serial_Low();
		}
		FEB_Fan_Clock_Pulse();
	}
}

uint32_t FEB_Fan_Read_Tachometer(uint8_t multiplex) {
	ADC_HandleTypeDef* hadc_pointer;
	if (multiplex == 0) {
		hadc_pointer = &hadc1;
	} else if (multiplex == 1) {
		hadc_pointer = &hadc2;
	}
	HAL_ADC_PollForConversion(hadc_pointer, 10);
	return HAL_ADC_GetValue(hadc_pointer);
}

void FEB_Fan_Validate_Speed(uint32_t adc_value, uint8_t fan_bank) {
	if (adc_value < FEB_Fan_ADC_Value_Limit) {
		switch (fan_bank) {
			case 1:
				FEB_Fan_1_Speed_Set(0);
				break;
			case 2:
				FEB_Fan_2_Speed_Set(0);
				break;
			case 3:
				FEB_Fan_3_Speed_Set(0);
				break;
			case 4:
				FEB_Fan_4_Speed_Set(0);
				break;
		}
	}
}
