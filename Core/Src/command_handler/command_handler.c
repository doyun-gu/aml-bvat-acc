/**
  ******************************************************************************
  * @file    command_handler.c
  * @brief   This file handles high-level command logic, timing for measurements,
  * and triggering data acquisition.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "command_handler.h"
#include "i2c_handler.h"  // For readACC() and LIS3DH constants
#include "uart_handler.h" // For WriteUART_Blocking()
#include "gpio_handler.h" // For g_is_logging_active
#include "timer.h"        // For timer_handler()
#include <stdio.h>        // For sprintf

/* Private define ------------------------------------------------------------*/
#define MEASUREMENT_INTERVAL_MS 100 // Interval for triggering accelerometer reads in ms

/* Private variables ---------------------------------------------------------*/
static u32 s_measurement_timer = 0;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initializes command handler specific components.
 * @param None
 * @retval None
 */
void CommandHandler_Init(void) {
    s_measurement_timer = HAL_GetTick();      // Initialize measurement timer
}

/**
 * @brief Performs detailed verification of the LIS3DH sensor.
 * @note  This function should be called after BVAT_I2C_Init() to confirm the sensor
 * is properly configured and responding.
 * @param None
 * @retval bool: true if sensor verification is successful, false otherwise.
 */
bool Verify_LIS3DH_Sensor(void) {
    char diag_buffer[128]; // Buffer for diagnostic messages via UART

    WriteUART_Blocking("Verifying LIS3DH Sensor State...\r\n");
    HAL_Delay(10); // Small delay before reading back

    // 1. Verify WHO_AM_I Register to confirm we are talking to the correct device
    u8 who_am_i = readACC(LIS3DH_REG_WHO_AM_I); // readACC is a blocking read
    sprintf(diag_buffer, "Read WHO_AM_I: 0x%02X (Expected: 0x%02X)\r\n", who_am_i, LIS3DH_WHO_AM_I_EXPECTED_VAL);
    WriteUART_Blocking(diag_buffer);

    // 2. Verify CTRL_REG1 Register to confirm our configuration write was successful
    u8 ctrl_reg1 = readACC(LIS3DH_REG_CTRL_REG1);
    sprintf(diag_buffer, "Read CTRL_REG1: 0x%02X (Expected: 0x%02X)\r\n", ctrl_reg1, ACC_CTRL_REG1_CONFIG_100HZ);
    WriteUART_Blocking(diag_buffer);

    // 3. Check if both readings are correct
    if (who_am_i == LIS3DH_WHO_AM_I_EXPECTED_VAL && ctrl_reg1 == ACC_CTRL_REG1_CONFIG_100HZ) {
        WriteUART_Blocking("SUCCESS: LIS3DH sensor verification passed!\r\n");
        return true;
    } else {
        WriteUART_Blocking("ERROR: LIS3DH verification failed. Check configuration or connection.\r\n");
        return false;
    }
}


/**
 * @brief Main measurement loop logic.
 * Checks if logging is active and periodically triggers an accelerometer data read.
 * @param None
 * @retval None
 */
void measure_loop(void) {
    // g_is_logging_active is a volatile bool externed from gpio_handler.h
    // It is controlled by the push-button interrupt callback.
    if (g_is_logging_active) {

        // Check timer for periodic measurement trigger.
        // This decouples the measurement rate from the main loop speed.
        if (timer_handler(MEASUREMENT_INTERVAL_MS, &s_measurement_timer)) {
            // It's time to take a measurement.
            // read_I2C_Blocking() will handle the I2C read and UART printout of the data.
            read_I2C_Blocking();
        }
    }
    // If logging is not active, this function does nothing, and the main
    // loop can perform its idle tasks (like the heartbeat blink).
}
