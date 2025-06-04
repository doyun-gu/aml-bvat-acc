/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 Aston Martin Lagonda.
  * All rights reserved.
  *
  * This firmware 
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "aml_hal.h"      // For u8, Error_Handler etc.
#include "gpio_handler.h" // For BuiltinLED_Init, indicate_setup_with_blinks, g_builtinLed
#include "uart_handler.h" // For WriteUART_Blocking
#include "timer.h"
#include "command_handler.h"
#include "i2c_handler.h"  // For BVAT_I2C_Init, readACC, read_I2C_Blocking, LIS3DH_REG_WHO_AM_I etc.
#include <stdio.h>        // For sprintf
#include <string.h>       // For strlen

// UART_HandleTypeDef huart2; // Should be extern from uart_handler.h

int main(void)
{
  // HAL initialisation
  BVAT_Init(); // This calls AML_Init -> MX_..._Inits (including I2C and UART)
  BuiltinLED_Init();

  HAL_Delay(100);
  WriteUART_Blocking("Hello from STM32 AML-BVAT!\r\n");

  bool i2c_system_fully_ok = false;
  char diag_buffer[128]; // Buffer for diagnostic messages

  WriteUART_Blocking("Starting I2C initialization and sensor check...\r\n");
  HAL_Delay(100);
  if (BVAT_I2C_Init()) { // This now calls the blocking I2C_ACC_Enable
                         // and I2C_connectivity_check (which prints its own status)

    // After BVAT_I2C_Init, let's verify sensor state by reading registers
    HAL_Delay(10); // Small delay after enabling sensor

    u8 who_am_i = readACC(LIS3DH_REG_WHO_AM_I); // readACC is blocking
    sprintf(diag_buffer, "LIS3DH WHO_AM_I: 0x%02X (Expected: 0x%02X)\r\n", who_am_i, LIS3DH_WHO_AM_I_EXPECTED_VAL);
    WriteUART_Blocking(diag_buffer);

    u8 ctrl_reg1 = readACC(LIS3DH_REG_CTRL_REG1);
    sprintf(diag_buffer, "LIS3DH CTRL_REG1: 0x%02X (Expected: 0x%02X)\r\n", ctrl_reg1, ACC_CTRL_REG1_CONFIG_100HZ);
    WriteUART_Blocking(diag_buffer);

    if (who_am_i == LIS3DH_WHO_AM_I_EXPECTED_VAL && ctrl_reg1 == ACC_CTRL_REG1_CONFIG_100HZ) {
        WriteUART_Blocking("LIS3DH appears configured and ready!\r\n");
        i2c_system_fully_ok = true;
    } else {
        WriteUART_Blocking("LIS3DH configuration mismatch or WHO_AM_I incorrect. Data reads might fail.\r\n");
        i2c_system_fully_ok = false; // Or keep it as per I2C_connectivity_check's result
    }
  } else {
    WriteUART_Blocking("BVAT_I2C_Init reported failure (connectivity or enable).\r\n");
    i2c_system_fully_ok = false;
  }

  indicate_setup_with_blinks(3, 100); // Visual cue

  if (i2c_system_fully_ok) {
    WriteUART_Blocking("AML BVAT ready for measurements.\r\n");
  } else {
    WriteUART_Blocking("AML BVAT I2C sensor issue. Measurements may not be accurate.\r\n");
  }

  while (1) {
    if (i2c_system_fully_ok) { // Only attempt to read if setup was okay
        read_I2C_Blocking(); // This function prints its own success/fail/data
    } else {
        // Optionally, periodically retry I2C init or just don't read
        WriteUART_Blocking("Skipping I2C read due to earlier errors.\r\n");
    }
    indicate_setup_with_blinks(3, 100); // Blinks LEDs and adds delay
  }
}