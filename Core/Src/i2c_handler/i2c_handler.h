#pragma once

#include <stdbool.h>
#include "aml_hal.h"
#include "uart_handler.h"
#include "stm32f4xx_hal.h"

extern I2C_HandleTypeDef hi2c1;
extern volatile bool acc_enabled;

// void MX_TIM2_Init(void);
void MX_I2C1_Init(void);

// === LIS3DH Accelerometer Constants ===
#define ACC_I2C_ADDR      (0x18 << 1)   // LIS3DH default 7-bit address shifted left
#define ACC_CTRL1_REG     0x20          // Control register for power + axes
#define ACC_CTRL1_VALUE   0x57          // 100Hz, all axes enabled
#define ACC_DATA_START    (0x28 | 0x80) // Start at OUT_X_L with auto-increment

extern u8 rawData[6]; // Accessible from other files if needed

// void I2C_connectivity_check(void);
bool I2C_connectivity_check(void);
void I2C_ACC_Enable(void);
void I2C_Read_ACC(void);
u8 readACC(u8 reg);
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);