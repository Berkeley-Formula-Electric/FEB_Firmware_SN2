// ********************************** Includes **********************************

#include "FEB_BMS_Shutdown.h"

extern CAN_HandleTypeDef hcan2;

// ********************************** Functions **********************************

void FEB_BMS_Shutdown_Startup(void) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
}

void FEB_BMS_Shutdown_Initiate(void) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

	if (FEB_CAN_CHARGER_CHARGE_BOOL) {
		FEB_CAN_Charger_Stop_Charge(&hcan2);
	}
}
