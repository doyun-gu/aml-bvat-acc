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

  HAL_Delay(100);
  WriteUART_Blocking("Hello from STM32 AML-BVAT!\r\n");

  WriteUART ("Starting I2C initialization...\r\n");

  
  HAL_Delay(100);
  BVAT_I2C_Init(); // Initialize I2C peripheral

  // Toggle LED for the first time to inidciate everything set up
  indicate_setup_with_blinks(3, 100);

  /**
   * !TESTER SPACE =============================================================
   */

  // --- UART Test: Send a message AFTER blinking and BEFORE the main loop ---
  // WriteUART("System Initialized. Blinking complete. UART OK!\r\n");

  // write a UART message to test
  // const char message_to_send[] = "Tester message\r\n"; // Added \r\n for new line
  // HAL_UART_Transmit_IT(&huart2, (uint8_t *)message_to_send, strlen(message_to_send));

  // WriteUART_Blocking_Test("Blocking UART Test OK!\r\n");
  // HAL_Delay(100); // Just to ensure it's not immediately followed by something else

  // You might add a small delay to ensure the message has time to physically transmit,
  // especially if the next operations are very fast or if you hit a breakpoint immediately.
  HAL_Delay(100); // e.g., 100ms delay

  // uint8_t uart_test_message[] = "Hello from STM32 AML-BVAT Initialised!\r\n";
  HAL_StatusTypeDef uart_tx_status;

  // // Transmit the message using HAL_UART_Transmit (blocking mode)
  // // Make sure huart2 has been initialized by MX_USART2_UART_Init() before this line.
  // uart_tx_status = HAL_UART_Transmit(&huart2,        // Pointer to your UART handle
  //                                    uart_test_message,  // Pointer to data buffer
  //                                    strlen((char*)uart_test_message), // Length of data
  //                                    1000);            // Timeout in milliseconds (e.g., 1000ms)

  // // Optional: Check the status of the transmission
  // if (uart_tx_status == HAL_OK)
  // {
  //   // Transmission was successful!
  //   // You could toggle an LED here differently to confirm, or just rely on seeing it in picocom.
  // }
  // else
  // {
  //   // Transmission failed (e.g., HAL_ERROR, HAL_TIMEOUT, HAL_BUSY)
  //   // The program will likely go into Error_Handler() if HAL_UART_Init failed earlier,
  //   // or if there's a hardware fault during transmit.
  //   // If HAL_UART_Init was OK but this fails, it's a more specific transmit issue.
  //   // For now, if this fails, check your Error_Handler for clues.
  //   // You could add a specific error indication here if Error_Handler isn't hit.
  //   Error_Handler(); // Or a specific error routine for UART TX fail
  // }
  HAL_Delay(10);
  WriteUART_Blocking("AML BVAT is ready for mueasrements.\r\n");

  //! TESTER SPACE END =========================================================

  while (1) {
    // measure_loop(); // Main measurement loop

    indicate_setup_with_blinks(3, 100);
  }
}