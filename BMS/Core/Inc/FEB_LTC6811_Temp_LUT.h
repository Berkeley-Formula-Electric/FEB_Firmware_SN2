#ifndef INC_FEB_LTC6811_TEMP_LUT_H_
#define INC_FEB_LTC6811_TEMP_LUT_H_

// ********************************** Includes **********************************

#include <math.h>
#include <stdint.h>

// ********************************** LUT Configuration **********************************

#define FEB_LTC6811_TEMP_LUT_MAP_SIZE 1141
#define FEB_LTC6811_TEMP_LUT_RESOLUTION 0.001
#define FEB_LTC6811_TEMP_LUT_MIN_MAP_TEMP -40.0
#define FEB_LTC6811_TEMP_LUT_MAX_MAP_TEMP 120.0
#define FEB_LTC6811_TEMP_LUT_MIN_MAP_VOLT 1.3
#define FEB_LTC6811_TEMP_LUT_MAX_MAP_VOLT 2.44

// ********************************** Functions **********************************

float FEB_LTC6811_Temp_LUT_Get_Temperature(float voltage);

#endif /* INC_FEB_LTC6811_TEMP_LUT_H_ */
