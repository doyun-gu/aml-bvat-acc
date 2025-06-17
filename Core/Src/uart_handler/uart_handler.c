/* Includes ------------------------------------------------------------------*/
#include "uart_handler.h"
#include <string.h>
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx; // Definition for the USART1 RX DMA handle

/* Function Definitions ------------------------------------------------------*/

void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

// ... (WriteUART_Blocking, WriteUART, and HAL_UART_TxCpltCallback remain the same)
void WriteUART_Blocking(const char *message)
{
    if (message == NULL) return;
    uint16_t len = strlen(message);
    if (len == 0) return;
    HAL_UART_Transmit(&huart2, (uint8_t *)message, len, HAL_MAX_DELAY);
}

HAL_StatusTypeDef WriteUART(const char *message) {
    if (message == NULL) return HAL_ERROR;
    uint16_t message_len = strlen(message);
    if (message_len == 0) return HAL_OK;
    if (HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_TX || HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_TX_RX) {
        return HAL_BUSY;
    }
    return HAL_UART_Transmit_IT(&huart2, (uint8_t *)message, message_len);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        // Handle transmit complete for PC logging if needed
    }
}
