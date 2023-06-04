// ********************************** Includes **********************************

#include "FEB_BMS_Shutdown.h"

extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart2;

// ********************************** Functions **********************************

void FEB_BMS_Shutdown_Startup(void) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
}

void FEB_BMS_Shutdown_Initiate(char shutdown_message[]) {
	// Shutdown Circuit
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

	// Broadcast Message
	char str[128];
	sprintf(str, "Shutdown: %s.", shutdown_message);
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);

	// Stop Discharge
	if (FEB_LTC6811_Balance_Cells_State == 1) {
		FEB_LTC6811_Clear_Balance_Cells();
	}

	// Stop charge
	if (FEB_CAN_CHARGER_START_CHARGE == 1) {
		FEB_CAN_Charger_Stop_Charge(&hcan1);
	}

	// Do nothing
	while (1) {}
}
