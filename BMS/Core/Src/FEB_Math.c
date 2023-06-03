// ********************************** Includes & External **********************************

#include "FEB_Math.h"

// ********************************** Functions **********************************

long unsignedToSignedLong(uint32_t value) {
	return (value & 0x7FFFFFFF) - (value & (0b1 << 31));
}

int16_t unsignedToSigned16(uint16_t value) {
	return (value & 0x7FFF) - (value & (0b1 << 15));
}
