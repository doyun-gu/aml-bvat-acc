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

/* Private variables ---------------------------------------------------------*/
// Global state variables like g_builtinLed, g_is_logging_active, etc. are
// defined in their respective handler files and declared 'extern' in their headers.

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  bool system_ready_for_logging = false;

  // --- 1. Core System & Peripheral Initialization ---
  AML_Init();             // Initializes HAL, Clocks, GPIO, UART, I2C, Timers.
  BuiltinLED_Init();        // Initializes the software struct for the built-in LED.
  BuiltinPushButton_Init(); // Initializes the software struct and hardware for the user button interrupt.
  CommandHandler_Init();    // Initializes timers or states needed by the command handler.

  // --- 2. Startup Messaging and Diagnostics ---
  HAL_Delay(100); // Give peripherals a moment to stabilize after power-on.
  WriteUART_Blocking("---------------------------------------\r\n");
  WriteUART_Blocking("System Boot: Hello from STM32 AML-BVAT!\r\n");
  WriteUART_Blocking("---------------------------------------\r\n");

  // --- 3. I2C System Initialization and Sensor Verification ---
  if (BVAT_I2C_Init()) { // This function initializes the I2C bus, pings the sensor, and enables it.
                         // It prints its own detailed status messages.

      // Now, perform a detailed verification of the sensor's configuration.
      if (Verify_LIS3DH_Sensor()) { // This new function is in command_handler.c
          system_ready_for_logging = true;
      }
  }
  
  // --- 4. Final Setup Indication ---
  indicate_setup_with_blinks(3, 100); // Visual cue that setup sequence is complete.

  if (system_ready_for_logging) {
    WriteUART_Blocking("System ready. Press USER button to start/stop logging.\r\n");
  } else {
    WriteUART_Blocking("System setup complete, but I2C SENSOR ISSUE DETECTED.\r\n");
  }
  WriteUART_Blocking("---------------------------------------\r\n");

  // --- 5. Main Execution Loop ---
  while (1)
  {
    // The main operational logic is now handled by measure_loop().
    // It internally checks if logging is active.
    measure_loop();
    
    // If logging is NOT active, perform an idle "heartbeat" blink.
    // This shows the system is alive and waiting for a button press.
    if (!g_is_logging_active) {
        HAL_GPIO_TogglePin(g_builtinLed.pin.port, g_builtinLed.pin.pin);
        HAL_Delay(500); // Slow heartbeat blink
    }
  }
}