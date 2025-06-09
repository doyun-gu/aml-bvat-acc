#pragma once

//==============================================================================
// Includes
//==============================================================================
// Standard C Includes
#include <stdio.h>      // For snprintf if used (though removed from .c for now)
#include <string.h>     // For strlen if used
#include <stdbool.h>    // For bool type

// STM32 HAL Includes
#include "stm32f4xx_hal.h"

// Custom Project HAL/Type Includes
#include "aml_hal.h"       // For u32, u8 etc.

// Other Project Modules
#include "timer.h"         // For timer_handler
#include "uart_handler.h"  // For WriteUART (though direct calls removed from .c)
#include "i2c_handler.h"   // For I2C_ACC_Enable, I2C_Read_ACC, acc_enabled
#include "gpio_handler.h"  // If any direct GPIO manipulation was needed (not currently)
// #include "file_handler.h" // For SD card logging (currently commented out)

//==============================================================================
// Function Prototypes
//==============================================================================

/**
 * @brief Initializes command handler specific components (e.g., timers).
 * @param None
 * @retval None
 */
void CommandHandler_Init(void);

/**
 * @brief Sets the status of I2C module readiness.
 * @param is_ready: true if the I2C peripheral and target device are initialized and ready, false otherwise.
 * @retval None
 */
void CommandHandler_SetI2CReady(bool is_ready);

/**
 * @brief Main measurement loop logic.
 * Checks I2C and accelerometer status, and periodically triggers an accelerometer data read.
 * Actual data processing and UART logging of accelerometer data is expected to occur
 * within I2C HAL callbacks (e.g., HAL_I2C_MemRxCpltCallback in i2c_handler.c).
 * @param None
 * @retval None
 */
void measure_loop(void);

// void log_to_sd(u32 timestamp, u8 x, u8 y, u8 z); // Kept commented as per original

// #endif /* COMMAND_HANDLER_H */