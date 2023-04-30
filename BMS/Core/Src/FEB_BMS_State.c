// ********************************** Includes & External **********************************

#include "FEB_BMS_State.h"

// ********************************** Functions **********************************

void FEB_BMS_State_Validate(void) {
	if (FEB_CAN_CHARGER_START_CHARGE == 1 && FEB_LTC6811_Balance_Cells_State == 1) {
		FEB_BMS_Shutdown_Initiate();
	}
}
