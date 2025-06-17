/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body for the final AML-BVAT Data Logger.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "aml_hal.h"
#include "gpio_handler.h"
#include "uart_handler.h"
#include "i2c_handler.h"
#include "gps_handler.h"
#include "command_handler.h"
#include <stdbool.h>

/* Private variables ---------------------------------------------------------*/
// All global state variables are managed in their respective handler modules.

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  bool system_ready = false;

  // --- 1. Core System & Peripheral Initialization ---
  AML_Init();             // Initializes HAL, Clocks, all peripherals (GPIO, UARTs, I2C, Timers).
  BuiltinLED_Init();
  BuiltinPushButton_Init(); // Initializes the hardware and software for the user button interrupt.
  CommandHandler_Init();    // Initializes timers/states needed by the command handler.
  GPS_Init();
  // --- 2. Startup Messaging & Sensor Verification ---
  HAL_Delay(100);
  WriteUART_Blocking("---------------------------------------\r\n");
  WriteUART_Blocking("System Boot: AML-BVAT Data Logger\r\n");
  WriteUART_Blocking("---------------------------------------\r\n");

  if (BVAT_I2C_Init()) { // This function pings the sensor and enables it.
      if (Verify_LIS3DH_Sensor()) { // This function reads registers to confirm configuration.
          // Both I2C init and sensor verification passed.
          // Now check GPS. Check_GPS_Status will handle its own messaging.
          Check_GPS_Status();  // Try it, but don't block
          system_ready = true; // Always allow logging even without GPS fix
      }
  }
  
  // --- 3. Final Setup Indication ---
  indicate_setup_with_blinks(3, 100); // Visual cue that all checks are complete.

  if (system_ready) {
    WriteUART_Blocking("System ready. Press USER button to start/stop logging.\r\n");
  } else {
    WriteUART_Blocking("CRITICAL ERROR: A sensor failed to initialize. Halting.\r\n");
    Error_Handler(); // Halt if sensors are not working.
  }
  WriteUART_Blocking("---------------------------------------\r\n");
  
  // --- 4. Main Execution Loop ---
  while (1)
  {
    // All periodic logic is now handled by this single function.
    // It contains the state machine for idle vs. logging states.
    measure_loop();
  }
}
