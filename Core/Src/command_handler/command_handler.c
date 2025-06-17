/**
  ******************************************************************************
  * @file    command_handler.c
  * @brief   This file handles the main application logic and state machine.
  ******************************************************************************
  */

#include "command_handler.h"
#include "i2c_handler.h"
#include "uart_handler.h"
#include "timer.h"
#include "gps_handler.h"
#include <stdio.h>
#include <string.h>

/* External debug flag for GPS output */
extern bool g_gps_debug_output_enabled;

/* Defines ------------------------------------------------------------*/
#define MEASUREMENT_INTERVAL_MS 1000  // Log a data packet once per second
#define IDLE_BLINK_INTERVAL_MS  1000  // Blink the LED once per second in idle mode
#define GPS_FIX_TIMEOUT_MS      300000 // 5 minutes

/* Private Variables ---------------------------------------------------------*/
static u32 s_measurement_timer = 0;
static u32 s_idle_blink_timer = 0;
static SensorData_t s_latest_sensor_data; // A single struct to hold all sensor data

/* Private Function Prototypes -----------------------------------------------*/
static void get_all_sensor_data(void);
static void log_all_sensor_data(void);
static void handle_idle_state(void);
static void handle_logging_state(void);


/* Public Functions --------------------------------------------------------*/

void CommandHandler_Init(void) {
    s_measurement_timer = HAL_GetTick();
    s_idle_blink_timer = HAL_GetTick();
    memset(&s_latest_sensor_data, 0, sizeof(SensorData_t));
}

void measure_loop(void) {
    // The GPS DMA buffer needs to be processed continuously in the background
    // regardless of the logging state, to keep it from overflowing.
    GPS_Process_Buffer();

    // This is the core state machine.
    if (g_is_logging_active) {
        handle_logging_state();
    } else {
        handle_idle_state();
    }
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

bool Check_GPS_Status(void) {
    uint32_t start_time = HAL_GetTick();
    WriteUART_Blocking("Waiting for GPS fix... (This can take several minutes)\r\n");

    uint8_t retry_count = 0;

    while (retry_count < 50) {  // ~5 seconds
        GPS_Process_Buffer();
        if (GPS_Get_Latest_Data(NULL)) {
            WriteUART_Blocking("SUCCESS: GPS fix acquired.\r\n");
            WriteDigitalOut(&g_builtinLed, 0);
            return true;
        }
        HAL_GPIO_TogglePin(g_builtinLed.pin.port, g_builtinLed.pin.pin);
        HAL_Delay(100);
        retry_count++;
    }

    WriteUART_Blocking("ERROR: GPS fix not detected. Continuing without GPS.\r\n");
    WriteDigitalOut(&g_builtinLed, 0);
    return false;
}

/* Private Functions --------------------------------------------------------*/

static void handle_idle_state(void) {
    g_gps_debug_output_enabled = false;  // Disable GPS debug output

    // Slow LED heartbeat to show system is idle
    if (timer_handler(IDLE_BLINK_INTERVAL_MS, &s_idle_blink_timer)) {
        HAL_GPIO_TogglePin(g_builtinLed.pin.port, g_builtinLed.pin.pin);
    }
}

static void handle_logging_state(void) {
    g_gps_debug_output_enabled = true;  // Enable GPS debug output

    WriteDigitalOut(&g_builtinLed, 1); // Solid LED ON
    if (timer_handler(MEASUREMENT_INTERVAL_MS, &s_measurement_timer)) {
        get_all_sensor_data();
        log_all_sensor_data();
    }
}

static void get_all_sensor_data(void) {
    GPS_Get_Latest_Data(&s_latest_sensor_data.gps_data);

    bool success = LIS3DH_Read_Accel_Data(
        &s_latest_sensor_data.accel_x_g,
        &s_latest_sensor_data.accel_y_g,
        &s_latest_sensor_data.accel_z_g);

    if (!success) {
        WriteUART_Blocking("WARNING: LIS3DH_Read_Accel_Data returned false\r\n");
        s_latest_sensor_data.accel_x_g = 0.0f;
        s_latest_sensor_data.accel_y_g = 0.0f;
        s_latest_sensor_data.accel_z_g = 0.0f;
    }
}

static void log_all_sensor_data(void) {
    char buffer[256];

    // Convert accelerometer values from g to mg
    int16_t accel_x_mg = (int16_t)(s_latest_sensor_data.accel_x_g * 1000.0f);
    int16_t accel_y_mg = (int16_t)(s_latest_sensor_data.accel_y_g * 1000.0f);
    int16_t accel_z_mg = (int16_t)(s_latest_sensor_data.accel_z_g * 1000.0f);

    // Convert speed to integer
    int16_t speed_int = (int16_t)s_latest_sensor_data.gps_data.speed_kmh;

    // Format and print the final data string (clean version)
    sprintf(buffer, "DATA, Accel(X:%d, Y:%d, Z:%d), GPS(Fix:%d, Spd:%d)\r\n",
        accel_x_mg,
        accel_y_mg,
        accel_z_mg,
        s_latest_sensor_data.gps_data.is_valid_fix,
        speed_int
    );

    WriteUART_Blocking(buffer);
}

