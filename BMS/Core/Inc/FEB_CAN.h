#pragma once

#include "stm32f4xx_hal.h"
#include "FEB_logger.h"

void FEB_CAN_initFilter(CAN_HandleTypeDef *CANx, uint32_t filter_id, uint32_t filter_mask);

HAL_StatusTypeDef FEB_CAN_transmit(CAN_HandleTypeDef *CANx, uint16_t can_id, uint8_t *data, uint16_t size, uint8_t is_blocking);
