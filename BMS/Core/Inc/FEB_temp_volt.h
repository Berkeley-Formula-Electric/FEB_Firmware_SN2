/*
 * FEB_temp_volt_map.h
 *
 *  Created on: Jan 21, 2023
 *      Author: aman
 */

#ifndef INC_FEB_TEMP_VOLT_H_
#define INC_FEB_TEMP_VOLT_H_

#include "FEB_temp_volt_map.h"

#define MAP_COUNT 1618

float RESOLUTION = 0.5;
float MIN_MAP_TEMP = -40.0;
float MAX_MAP_TEMP = 120.0;
float MIN_MAP_VOLT = 1.3;
float MAX_MAP_VOLT = 2.44;

float getTemperature(uint16_t val) {
	float val_raw = val * 0.0001;
	if (val == 65535 || val_raw < MIN_MAP_VOLT || val_raw > MAX_MAP_VOLT) {
		return -42;
	}

	// find nearest value
	int index = 0;
	uint8_t found = 0;
	while (index < MAP_COUNT - 1 && !found) {
		if (TEMP_VOLT_MAP[index][1] >= val_raw && val_raw >= TEMP_VOLT_MAP[index + 1][1]) {
			found = 1;
		}
		index++;
	}

	if (val_raw - TEMP_VOLT_MAP[index][1] < 0.5) {
		return TEMP_VOLT_MAP[index][0];
	} else {
		return TEMP_VOLT_MAP[index + 1][0];
	}
}

#endif /* INC_FEB_TEMP_VOLT_H_ */
