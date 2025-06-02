/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "aml_hal.h"
#include "gpio_handler.h"
#include "uart_handler.h"
#include "timer.h"
#include "command_handler.h"
#include "i2c_handler.h"
#include "uart_handler.h"

/* Private variables ---------------------------------------------------------*/
// UART_HandleTypeDef huart2;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  // HAL initialisation
  BVAT_Init();
  BuiltinLED_Init();

  // Toggle LED for the first time to inidciate everything set up
  indicate_setup_with_blinks(3, 100);

  while (1) {
    measure_loop(); // Main measurement loop
  }
}