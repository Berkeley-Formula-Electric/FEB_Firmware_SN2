// ********************************** Includes & External **********************************

#include "FEB_Timer.h"

extern TIM_HandleTypeDef htim4;

// ********************************** Initialize **********************************

void FEB_Timer_Init(void) {
	HAL_TIM_Base_Start(&htim4);
}

// ********************************** Functions **********************************

void FEB_Timer_Delay_Micro(uint16_t delay) {
	__HAL_TIM_SET_COUNTER(&htim4, 0);
	while (__HAL_TIM_GET_COUNTER(&htim4) < delay);
}
