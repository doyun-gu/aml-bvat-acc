/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body for AML BVAT Project.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "aml_hal.h"
#include "gpio_handler.h"
#include "uart_handler.h"
#include "command_handler.h"
#include "i2c_handler.h"
#include "gps_handler.h"
#include <stdbool.h>

/* External Flags -----------------------------------------------------------*/
extern volatile bool gps_data_ready_flag; // Declared in gps_handler.c

/* Private variables ---------------------------------------------------------*/
bool i2c_ok = false;
bool gps_ok = false;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  // --- 1. Core System & Peripheral Initialization ---
  AML_Init();             // Initializes HAL, Clocks, all peripherals (GPIO, UARTs, I2C, Timers, GPS DMA).
  BuiltinLED_Init();
  BuiltinPushButton_Init();
  CommandHandler_Init();

  // --- 2. Startup Messaging ---
  HAL_Delay(100); // Give peripherals a moment to stabilize after power-on.
  WriteUART_Blocking("---------------------------------------\r\n");
  WriteUART_Blocking("System Boot: Hello from STM32 AML-BVAT!\r\n");
  WriteUART_Blocking("---------------------------------------\r\n");

  // --- 3. I2C Accelerometer Verification ---
  WriteUART_Blocking("--- Checking I2C Accelerometer ---\r\n");
  if (BVAT_I2C_Init()) {
      if (Verify_LIS3DH_Sensor()) {
          i2c_ok = true;
      }
  }

  // --- 4. GPS Module Verification ---
  WriteUART_Blocking("\r\n--- Checking GPS Module ---\r\n");
  gps_ok = Check_GPS_Status();

  // --- 5. Final Setup Indication & Readiness Report ---
  indicate_setup_with_blinks(3, 100);
  bool system_ready_for_logging = i2c_ok && gps_ok;

  if (system_ready_for_logging) {
      WriteUART_Blocking("System ready. Press USER button to start/stop logging.\r\n");
  } else {
      WriteUART_Blocking("System setup complete, but SENSOR ISSUE DETECTED.\r\n");
      if (!i2c_ok) { WriteUART_Blocking("- Accelerometer (I2C) FAILED\r\n"); }
      if (!gps_ok) { WriteUART_Blocking("- GPS FAILED (No fix or timeout)\r\n"); }
  }
  WriteUART_Blocking("---------------------------------------\r\n");

  // --- 6. Main Execution Loop ---
  while (1)
  {
    // --- GPS Data Processing Triggered by DMA ISR ---
    if (gps_data_ready_flag) {
        gps_data_ready_flag = false;
        GPS_Process_Buffer();
    }

    // --- Main Measurement ---
    if (system_ready_for_logging) {
        measure_loop();
    }

    // --- Idle Heartbeat if Not Logging ---
    if (!g_is_logging_active) {
        HAL_GPIO_TogglePin(g_builtinLed.pin.port, g_builtinLed.pin.pin);
        HAL_Delay(500);
    }
  }
}
