#ifndef INC_FEB_MATH_H_
#define INC_FEB_MATH_H_

// ********************************** Includes **********************************

#include "stdint.h"

// ********************************** Definitions **********************************

#define FEB_MATH_ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0]))

// ********************************** Functions **********************************

long FEB_Math_Uint32_To_Signed_Long(uint32_t value);
int16_t FEB_Math_Uint16_To_Int16(uint16_t value);

#endif /* INC_FEB_MATH_H_ */
