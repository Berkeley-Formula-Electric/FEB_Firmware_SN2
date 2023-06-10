#ifndef INC_FEB_TIMER_H_
#define INC_FEB_TIMER_H_

// ********************************** Includes **********************************

#include "stm32f4xx_hal.h"

// ********************************** Functions **********************************

void FEB_Timer_Init(void);
void FEB_Timer_Delay_Micro(uint16_t delay);

#endif /* INC_FEB_TIMER_H_ */
