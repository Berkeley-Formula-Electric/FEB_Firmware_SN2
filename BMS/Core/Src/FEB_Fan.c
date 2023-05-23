// ********************************** Includes & External **********************************

#include "FEB_Fan.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

// ********************************** Fan Configuration **********************************

static uint8_t FEB_Fan_1_Speed = 0;		// 0-255
static uint8_t FEB_Fan_2_Speed = 0;		// 0-255
static uint8_t FEB_Fan_3_Speed = 0;		// 0-255
static uint8_t FEB_Fan_4_Speed = 0;		// 0-255

// ********************************** PWM **********************************

void FEB_Fan_Init(void) {
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

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

// ********************************** Tachometer **********************************

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

void FEB_Fan_Set_Tachometer(uint8_t value) {
	for (uint8_t i = 0; i < 8; i++) {
		if ((value << i) & 0b1 == 1) {
			FEB_Fan_Serial_High();
		} else {
			FEB_Fan_Serial_Low();
		}
		FEB_Fan_Clock_High();
		// Todo: Add delay
		FEB_Fan_Clock_Low();
		// Todo: Add delay
	}
}
