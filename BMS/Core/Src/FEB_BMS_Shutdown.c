// ********************************** Includes **********************************

#include "FEB_BMS_Shutdown.h"

extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart2;

// ********************************** Functions **********************************

void FEB_BMS_Shutdown_Startup(void) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
}

void FEB_BMS_Shutdown_Initiate(void) {
	// Shutdown Circuit
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

	// Stop Charging
	if (FEB_CAN_CHARGER_CHARGE_BOOL) {
		FEB_CAN_Charger_Stop_Charge(&hcan1);
	}

	// Broadcast Message
	char str[128];
	sprintf(str, "Shutdown!\n");
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);

	// Do nothing
	while (1) {}
}
