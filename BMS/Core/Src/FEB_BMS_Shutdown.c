#include "FEB_BMS_Shutdown.h"
#include "stm32f4xx_hal.h"


void FEB_BMS_Shutdown_Startup(void) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
}

void FEB_BMS_Shutdown_Initiate(void) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
}
