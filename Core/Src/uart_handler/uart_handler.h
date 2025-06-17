#pragma once

//==============================================================================
// Includes
//==============================================================================
#include "stm32f4xx_hal.h"
#include "aml_hal.h"

//==============================================================================
// Extern Variable Declarations
//==============================================================================
extern UART_HandleTypeDef huart1; // Handle for USART1 (used by GPS)
extern UART_HandleTypeDef huart2; // Handle for USART2 (used for VCP to PC)
extern DMA_HandleTypeDef hdma_usart1_rx; // DMA Handle for USART1 Receive

//==============================================================================
// Function Prototypes
//==============================================================================

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void WriteUART_Blocking(const char *message);
HAL_StatusTypeDef WriteUART(const char *message);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
