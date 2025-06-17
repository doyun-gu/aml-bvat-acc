#pragma once

//==============================================================================
// Includes
//==============================================================================
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "aml_hal.h"
#include "gpio_handler.h"

//==============================================================================
// Function Prototypes
//==============================================================================

/**
 * @brief Initializes command handler specific components (e.g., timers).
 */
void CommandHandler_Init(void);

/**
 * @brief Performs detailed verification of the LIS3DH sensor after I2C initialization.
 * @retval bool: true if sensor verification is successful, false otherwise.
 */
bool Verify_LIS3DH_Sensor(void);

/**
 * @brief Checks for a valid GPS fix, blocking until one is acquired or a timeout occurs.
 * @note  This function provides visual feedback (blinking LED) while it is actively waiting.
 * @retval bool: true if a valid GPS fix was acquired, false if it timed out.
 */
bool Check_GPS_Status(void);

/**
 * @brief Main application loop logic, called repeatedly from main().
 */
void measure_loop(void);

// #endif /* COMMAND_HANDLER_H */
