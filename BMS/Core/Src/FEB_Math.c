// ********************************** Includes & External **********************************

#include "FEB_Math.h"

// ********************************** Functions **********************************

long FEB_Math_Uint32_To_Signed_Long(uint32_t value) {
	return (value & 0x7FFFFFFF) - (value & (0b1 << 31));
}

int16_t FEB_Math_Uint16_To_Int16(uint16_t value) {
	return (value & 0x7FFF) - (value & (0b1 << 15));
}
