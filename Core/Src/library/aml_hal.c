#include "aml_hal.h"

/**
 * HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET); // Turn on LED
 * HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);              // Toggle LED
 */


/**
 * @file aml_hal.c
 * This function initialises all necessary configurations for the Hardware Abstraction Layer (HAL)
 * on the STM32 board. It sets up the HAL library and prepares the microcontroller for further
 * hardware operations. Typically, this involves calling HAL_Init() and configuring system clocks,
 * peripherals, and interrupts as required.
 */
 
void AML_Init(void) {
    // Initialize the HAL Library
    HAL_Init();

    // Configure the system clock
    SystemClock_Config();

    // Initialize all configured peripherals
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_I2C1_Init();
    MX_TIM2_Init();
}

