#pragma once

#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "aml_hal.h"


extern UART_HandleTypeDef huart2;

void MX_USART2_UART_Init(void);
HAL_StatusTypeDef WriteUART(const char *message);
void WriteUART_Blocking(const char *message);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);