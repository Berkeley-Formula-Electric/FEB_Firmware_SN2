#ifndef INC_FEB_FAN_H_
#define INC_FEB_FAN_H_

// ********************************** Includes & External **********************************

#include "stm32f4xx_hal.h"

// ********************************** Multiplex Select Configuration **********************************

#define FEB_Fan_Multiplex_2_Shift_R 3

// Multiplex 1
#define FEB_Fan_TACH01 0
#define FEB_Fan_TACH02 1
#define FEB_Fan_TACH03 2
#define FEB_Fan_TACH04 3
#define FEB_Fan_TACH05 4
#define FEB_Fan_TACH06 5

// Multiplex 2
#define FEB_Fan_TACH07 0 << FEB_Fan_Multiplex_2_Shift_R
#define FEB_Fan_TACH08 1 << FEB_Fan_Multiplex_2_Shift_R
#define FEB_Fan_TACH09 2 << FEB_Fan_Multiplex_2_Shift_R
#define FEB_Fan_TACH10 3 << FEB_Fan_Multiplex_2_Shift_R
#define FEB_Fan_TACH11 4 << FEB_Fan_Multiplex_2_Shift_R
#define FEB_Fan_TACH12 5 << FEB_Fan_Multiplex_2_Shift_R


// ********************************** Functions **********************************

void FEB_Fan_Init(void);
void FEB_Fan_1_Speed_Set(uint8_t speed);
void FEB_Fan_2_Speed_Set(uint8_t speed);
void FEB_Fan_3_Speed_Set(uint8_t speed);
void FEB_Fan_4_Speed_Set(uint8_t speed);

#endif /* INC_FEB_FAN_H_ */
