#ifndef INC_FEB_FAN_H_
#define INC_FEB_FAN_H_

// ********************************** Includes & External **********************************

#include "stm32f4xx_hal.h"

// ********************************** Functions **********************************

void FEB_Fan_Init(void);
void FEB_Fan_1_Speed_Set(uint8_t speed);
void FEB_Fan_2_Speed_Set(uint8_t speed);
void FEB_Fan_3_Speed_Set(uint8_t speed);
void FEB_Fan_4_Speed_Set(uint8_t speed);

#endif /* INC_FEB_FAN_H_ */
