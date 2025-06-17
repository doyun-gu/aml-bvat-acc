/**
  ******************************************************************************
  * @file    command_handler.c
  * @brief   This file handles high-level command logic, timing for measurements,
  * and triggering data acquisition.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "command_handler.h"
#include "i2c_handler.h"
#include "uart_handler.h"
#include "timer.h"
#include "gps_handler.h"
#include <stdio.h>

/* Private define ------------------------------------------------------------*/
#define MEASUREMENT_INTERVAL_MS 1000 // Log a data packet once per second
#define GPS_FIX_TIMEOUT_MS      300000   // Timeout for getting a GPS fix (5 minutes)

/* Private variables ---------------------------------------------------------*/
static u32 s_measurement_timer = 0;

/* Exported functions --------------------------------------------------------*/

void CommandHandler_Init(void) {
    s_measurement_timer = HAL_GetTick();
}

bool Verify_LIS3DH_Sensor(void) {
    char diag_buffer[128];
    WriteUART_Blocking("Verifying LIS3DH Sensor State...\r\n");
    HAL_Delay(10);
    u8 who_am_i = readACC(LIS3DH_REG_WHO_AM_I);
    sprintf(diag_buffer, "Read WHO_AM_I: 0x%02X (Expected: 0x%02X)\r\n", who_am_i, LIS3DH_WHO_AM_I_EXPECTED_VAL);
    WriteUART_Blocking(diag_buffer);
    u8 ctrl_reg1 = readACC(LIS3DH_REG_CTRL_REG1);
    sprintf(diag_buffer, "Read CTRL_REG1: 0x%02X (Expected: 0x%02X)\r\n", ctrl_reg1, ACC_CTRL_REG1_CONFIG_100HZ);
    WriteUART_Blocking(diag_buffer);
    if (who_am_i == LIS3DH_WHO_AM_I_EXPECTED_VAL && ctrl_reg1 == ACC_CTRL_REG1_CONFIG_100HZ) {
        WriteUART_Blocking("SUCCESS: LIS3DH sensor verification passed!\r\n");
        return true;
    } else {
        WriteUART_Blocking("ERROR: LIS3DH verification failed.\r\n");
        return false;
    }
}

/**
 * @brief Checks for a valid GPS fix, with a timeout.
 * @note  This is a blocking function that will loop until a fix is acquired
 * or the timeout is reached. It provides visual feedback while searching.
 * @retval bool: true if a valid GPS fix was acquired, false if it timed out.
 */
bool Check_GPS_Status(void) {
    uint32_t start_time = HAL_GetTick();
    WriteUART_Blocking("Waiting for GPS fix... (This can take several minutes)\r\n");

    // Loop until we get a valid fix or we time out
    while (GPS_Get_Latest_Data(NULL) == false) {
        // Check for timeout
        if ((HAL_GetTick() - start_time) > GPS_FIX_TIMEOUT_MS) {
            WriteUART_Blocking("ERROR: Timed out waiting for GPS fix.\r\n");
            WriteDigitalOut(&g_builtinLed, 0); // Turn off blinking LED
            return false; // Return failure
        }

        // Provide visual "searching" feedback with a fast blink
        HAL_GPIO_TogglePin(g_builtinLed.pin.port, g_builtinLed.pin.pin);
        HAL_Delay(100); // Fast blink
    }

    // If we exit the loop, we have a fix!
    WriteUART_Blocking("SUCCESS: GPS fix acquired.\r\n");
    WriteDigitalOut(&g_builtinLed, 0); // Turn LED off to prepare for "ready" blinks
    return true; // Return success
}

void measure_loop(void) {
    if (g_is_logging_active) {
        if (timer_handler(MEASUREMENT_INTERVAL_MS, &s_measurement_timer)) {
            // In the final version, this will call a function that logs both accel and GPS
            read_I2C_Blocking();
            Log_Latest_GPS_Data();
        }
    }
}
