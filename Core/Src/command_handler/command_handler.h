#pragma once

//==============================================================================
// Includes
//==============================================================================
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "aml_hal.h"
#include "gpio_handler.h"
#include "gps_handler.h"   // For the GPS_Data_t struct definition

//==============================================================================
// Structs and Typedefs
//==============================================================================

/**
 * @brief A single, consolidated structure to hold one snapshot of all sensor data.
 */
typedef struct {
    GPS_Data_t gps_data; // The complete GPS data struct from the GPS handler
    float accel_x_g;     // Accelerometer X-axis in 'g's
    float accel_y_g;     // Accelerometer Y-axis in 'g's
    float accel_z_g;     // Accelerometer Z-axis in 'g's
} SensorData_t;


//==============================================================================
// Function Prototypes
//==============================================================================

/**
 * @brief Initializes the command handler and its timers.
 */
void CommandHandler_Init(void);

/**
 * @brief Performs detailed verification of the LIS3DH sensor.
 * @retval bool: true if sensor verification is successful, false otherwise.
 */
bool Verify_LIS3DH_Sensor(void);

/**
 * @brief Checks for a valid GPS fix, blocking until one is acquired or a timeout occurs.
 * @retval bool: true if a valid GPS fix was acquired, false if it timed out.
 */
bool Check_GPS_Status(void);

/**
 * @brief Main application loop logic, called repeatedly from main().
 * @note  This function acts as a state machine, handling both the "idle"
 * and "logging" states based on the g_is_logging_active flag.
 */
void measure_loop(void);

// #endif /* COMMAND_HANDLER_H */
