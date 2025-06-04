/**
  ******************************************************************************
  * @file    aml_hal.c
  * @brief   This file provides implementation for the AML (Aston Martin Lagonda)
  * Hardware Abstraction Layer utility functions, including main
  * initialization and error handling.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "aml_hal.h"      // Primary header for this module

// Include headers for all MX_..._Init functions and SystemClock_Config
#include "main.h"         // Often contains SystemClock_Config() prototype and may include others
#include "gpio_handler.h" // For MX_GPIO_Init() and g_builtinLed
#include "uart_handler.h" // For MX_USART2_UART_Init()
#include "i2c_handler.h"  // For MX_I2C1_Init()
#include "timer.h"        // For MX_TIM2_Init()

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
// No private variables defined in this file. Global variables like g_builtinLed
// are defined in their respective handler files (e.g., gpio_handler.c).

/* Private function prototypes -----------------------------------------------*/
// If SystemClock_Config is defined in main.c and not prototyped in main.h,
// you might need a prototype here, but it's better if main.h provides it.
// static void SystemClock_Config(void); // Example if it were static to this scope

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Initializes core HAL, system clocks, and base peripherals.
 * @note   This function should be called once at the beginning of main().
 * It calls HAL_Init(), SystemClock_Config(), and various MX_..._Init() functions.
 * @param  None
 * @retval None
 */
void AML_Init(void) {
    // 1. Initialize the HAL Library (mandatory first step)
    // This function:
    // - Configures the Flash prefetch, instruction and data caches.
    // - Sets the NVIC Group Priority.
    // - Initializes the SysTick timer to generate interrupts at 1ms intervals (used for HAL_Delay).
    HAL_Init();

    // 2. Configure the system clock
    // This function (typically defined in main.c by CubeMX) sets up the main
    // system clock (SYSCLK), HCLK, PCLK1, PCLK2.
    SystemClock_Config(); // Ensure this is prototyped, usually in main.h

    // 3. Initialize common/base peripherals
    // These MX_... functions should handle their respective HAL_..._Init()
    // and HAL_..._MspInit() calls.
    MX_GPIO_Init();       // Initialize general purpose I/O pins
    MX_USART2_UART_Init();// Initialize USART2 for serial communication
    MX_I2C1_Init();       // Initialize I2C1 for sensor communication
    MX_TIM2_Init();       // Initialize TIM2 (or other timers as needed)
    // Add other MX_..._Init() calls as your project requires
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @note   Halts the system in an infinite loop after disabling interrupts.
  * Attempts to turn on g_builtinLed if initialized.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  // Attempt to turn on an indicator LED.
  // g_builtinLed must be properly initialized by BuiltinLED_Init() before
  // Error_Handler is called for this to work.
  // The extern declaration for g_builtinLed comes from including "gpio_handler.h".
  if (g_builtinLed.pin.port != NULL) { // Basic check if g_builtinLed is somewhat initialized
      HAL_GPIO_WritePin(g_builtinLed.pin.port, g_builtinLed.pin.pin, GPIO_PIN_SET); // Turn LED ON SOLID
  }

  // Disable all interrupts to prevent further issues
  __disable_irq();

  // Infinite loop to halt the system
  while (1)
  {
    // Processor is halted here.
    // Attach a debugger to find the call stack leading to Error_Handler.
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  * where the assert_param error has occurred.
  * @param  file Pointer to the source file name.
  * @param  line assert_param error line source number.
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  // Example: Send assertion failure information over UART if UART is initialized
  // char assert_msg[100];
  // sprintf(assert_msg, "Assert failed: file %s, line %lu\r\n", file, line);
  // WriteUART_Blocking(assert_msg); // Assuming a blocking UART function for reliability here

   Error_Handler(); // Often, an assertion failure is critical enough to halt.
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */