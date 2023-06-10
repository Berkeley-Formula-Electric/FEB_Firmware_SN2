// ********************************** Includes & External **********************************

#include "FEB_BMS_Shutdown.h"

extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart2;

// ********************************** Global Variable **********************************

uint8_t FEB_BMS_Shutdown_State = 0;		// 0 - not shutdown, 1 - shutdown

// ********************************** Functions **********************************

void FEB_BMS_Shutdown_Startup(void) {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
}

void FEB_BMS_Shutdown_Initiate(char shutdown_message[]) {
	FEB_BMS_Shutdown_State = 1;

	// Shutdown Circuit
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

	// Broadcast Message
	char str[128];
	sprintf(str, "Shutdown: %s.", shutdown_message);
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);

	// Stop Discharge
	if (FEB_LTC6811_BALANCE_STATE == 1) {
		FEB_LTC6811_Clear_Balance_Cells();
	}

	// Stop charge
	if (FEB_CAN_CHARGER_STATE == 1) {
		FEB_CAN_Charger_Stop_Charge(&hcan1);
	}

	// Turn off fans
	FEB_Fan_All_Speed_Set(0);
}
