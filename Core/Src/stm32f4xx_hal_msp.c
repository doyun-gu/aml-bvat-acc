
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file         stm32f4xx_hal_msp.c
  * @brief        This file provides code for the MSP Initialization
  *               and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
}

/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART2)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
     HAL_NVIC_SetPriority(USART2_IRQn, 0, 0); // Set appropriate preemption and sub-priority
    HAL_NVIC_EnableIRQ(USART2_IRQn);         // Enable the USART2 global interrupt
  }
}

/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, USART_TX_Pin|USART_RX_Pin);
  }

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hi2c->Instance==I2C1) // Check if it's for I2C1
  {
    // 1. Enable clock for GPIOB (since PB8 and PB9 are on Port B)
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /**I2C1 GPIO Configuration
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9; // Specify PB8 and PB9
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;      // Alternate Function Open Drain (Critical for I2C)
    GPIO_InitStruct.Pull = GPIO_PULLUP;          // Enable internal pull-up (External pull-ups are often better/needed)
                                                 // Use GPIO_NOPULL if you have strong external pull-ups.
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // Or GPIO_SPEED_FREQ_HIGH
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;   // Alternate Function mapping for I2C1 on PB8/PB9 for STM32F4xx
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);      // Initialize GPIOB pins 8 and 9

    // 2. Enable I2C1 peripheral clock
    __HAL_RCC_I2C1_CLK_ENABLE();

    // 3. NVIC configuration for I2C1 interrupts (if you use IT functions, good to have)
    //    (Your MX_I2C1_Init function already includes this, which is also fine.
    //     It's common to see it either here or in the peripheral's main init function.
    //     If it's in MX_I2C1_Init, you don't strictly need to repeat it here, but ensure it's done.)
    /*
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0); // Or a different priority for errors
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
    */
  }
}