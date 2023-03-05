// ********************************** Includes **********************************

#include "FEB_BMS_Shutdown.h"

// ********************************** Functions **********************************

void FEB_BMS_Shutdown_Startup(void) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
}

void FEB_BMS_Shutdown_Initiate(void) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
}
