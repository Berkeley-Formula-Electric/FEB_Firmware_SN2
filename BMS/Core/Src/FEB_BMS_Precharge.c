// ********************************** Includes & External **********************************

#include "FEB_BMS_Shutdown.h"

extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart2;

// ********************************** Functions **********************************

void FEB_BMS_Precharge_Open(void) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
}

void FEB_BMS_Precharge_Close(void) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
}
