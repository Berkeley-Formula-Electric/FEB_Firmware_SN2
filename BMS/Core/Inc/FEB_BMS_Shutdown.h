#ifndef INC_FEB_BMS_SHUTDOWN_H_
#define INC_FEB_BMS_SHUTDOWN_H_

// ********************************** Includes **********************************

#include "string.h"
#include "stdio.h"

#include "FEB_LTC6811.h"
#include "FEB_CAN_Charger.h"
#include "FEB_Fan.h"

// ********************************** Functions **********************************

void FEB_BMS_Shutdown_Startup(void);						// Close relay
void FEB_BMS_Shutdown_Initiate(char shutdown_message[]);	// Open relay

#endif /* INC_FEB_BMS_SHUTDOWN_H_ */
