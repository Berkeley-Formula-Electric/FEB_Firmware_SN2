// ********************************** Includes & External **********************************

#include "FEB_BMS_State.h"

// ********************************** Functions **********************************

void FEB_BMS_State_Validate(void) {
	if (FEB_CAN_CHARGER_STATE == 1 && FEB_LTC6811_BALANCE_STATE == 1) {
		FEB_BMS_Shutdown_Initiate("Charge state and balance state enabled");
	}
}
