#include "FEB_logger.h"

extern UART_HandleTypeDef huart2;

void FEB_log(char *module, char *level, char *msg) {
  char str[256];
  sprintf(str, "<%s> [%s] %s\r\n", module, level, msg);
  HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 100);
}

