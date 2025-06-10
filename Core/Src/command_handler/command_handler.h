#pragma once

//==============================================================================
// Includes
//==============================================================================
#include <stdbool.h>      // For bool type
#include "stm32f4xx_hal.h" // For core HAL types
#include "aml_hal.h"       // For u32, u8 etc.

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
 * @brief Performs detailed verification of the LIS3DH sensor after I2C initialization.
 * @note  Reads the WHO_AM_I and configuration registers to confirm the sensor
 * is present, correct, and properly configured. Prints detailed
 * diagnostic messages via UART.
 * @param None
 * @retval bool: true if sensor verification is successful, false otherwise.
 */
bool Verify_LIS3DH_Sensor(void);

/**
 * @brief Main measurement loop logic.
 * Checks if logging is active and periodically triggers an accelerometer data read.
 * @param None
 * @retval None
 */
void measure_loop(void);

// #endif /* COMMAND_HANDLER_H */

