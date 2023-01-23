#include "FEB_CAN.h"

void FEB_CAN_initFilter(CAN_HandleTypeDef *CANx, uint32_t filter_id, uint32_t filter_mask) {
  CAN_FilterTypeDef filter_config;

  if (CANx->Instance == CAN1) {
    filter_config.FilterBank = 0;
  }
  else {
    filter_config.FilterBank = 14;
  }
  
  filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
  filter_config.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  filter_config.FilterIdHigh = filter_id << 5;
  filter_config.FilterIdLow = 0;
  filter_config.FilterMaskIdHigh = filter_mask << 5;
  filter_config.FilterMaskIdLow = 0;
  filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
  filter_config.FilterActivation = ENABLE;
  filter_config.SlaveStartFilterBank = 14;
  HAL_CAN_ConfigFilter(CANx, &filter_config);
}


HAL_StatusTypeDef FEB_CAN_transmit(CAN_HandleTypeDef *CANx, uint16_t can_id, uint8_t *data, uint16_t size, uint8_t is_blocking) {
  uint32_t mailbox;

  CAN_TxHeaderTypeDef header;
  header.DLC = size;
  header.IDE = CAN_ID_STD;
  header.RTR = CAN_RTR_DATA;
  header.StdId = can_id;
  header.TransmitGlobalTime = DISABLE;

  uint32_t tx_fifo_level = HAL_CAN_GetTxMailboxesFreeLevel(CANx);

  if (is_blocking) {
    while (tx_fifo_level == 0) {
      tx_fifo_level = HAL_CAN_GetTxMailboxesFreeLevel(CANx);
    }
  }
  else if (tx_fifo_level == 0) {
    FEB_log("FEB_CAN", "ERROR", "CAN busy");
    return HAL_BUSY;
  }

  if (HAL_CAN_AddTxMessage(CANx, &header, (uint8_t *)data, &mailbox) != HAL_OK) {
    FEB_log("FEB_CAN", "ERROR", "CAN TX error");
    return HAL_ERROR;
  }
  return HAL_OK;
}
