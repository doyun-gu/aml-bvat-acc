#pragma once

//==============================================================================
// Includes
//==============================================================================
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "aml_hal.h"
#include "uart_handler.h"

//==============================================================================
// LIS3DH Accelerometer Specific Constants
//==============================================================================
#define LIS3DH_7BIT_ADDR              (0x18)
#define ACC_I2C_ADDR                  (LIS3DH_7BIT_ADDR << 1)
#define LIS3DH_REG_WHO_AM_I           (0x0F)
#define LIS3DH_WHO_AM_I_EXPECTED_VAL  (0x33)
#define LIS3DH_REG_CTRL_REG1          (0x20)
#define ACC_CTRL_REG1_CONFIG_100HZ    (0x57)
#define LIS3DH_REG_OUT_X_L            (0x28)
#define LIS3DH_DATA_START_MULTI_READ  (LIS3DH_REG_OUT_X_L | 0x80)

//==============================================================================
// Extern Variable Declarations
//==============================================================================
extern I2C_HandleTypeDef hi2c1;
extern volatile bool acc_enabled;
extern u8 rawData[6];

//==============================================================================
// Function Prototypes
//==============================================================================
void MX_I2C1_Init(void);
bool BVAT_I2C_Init(void);
void Scan_I2C_Bus(void); // Function to scan the I2C bus for devices
bool I2C_connectivity_check(void);
void I2C_ACC_Enable(void);
void I2C_Read_ACC(void);
u8 readACC(u8 reg);

// This is the function that likely had a missing semicolon in your file
bool LIS3DH_Read_Accel_Data(float* x_g, float* y_g, float* z_g); // <<< --- FIX: Ensure semicolon is here.

// HAL Callback Prototypes
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);

bool Verify_LIS3DH_Sensor(void); // Function to verify LIS3DH sensor


// #endif /* I2C_HANDLER_H */
