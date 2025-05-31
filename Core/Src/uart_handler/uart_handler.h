#pragma once

#include <string.h>
#include "stm32f4xx_hal.h"
#include "aml_hal.h"


extern UART_HandleTypeDef huart2;

void MX_USART2_UART_Init(void);