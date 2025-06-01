#pragma once

#include <stdbool.h>
#include "aml_hal.h"
#include "stm32f4xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

// === LIS3DH Accelerometer Constants ===
#define ACC_I2C_ADDR      (0x18 << 1)   // LIS3DH default 7-bit address shifted left
#define ACC_CTRL1_REG     0x20          // Control register for power + axes
#define ACC_CTRL1_VALUE   0x57          // 100Hz, all axes enabled
#define ACC_DATA_START    (0x28 | 0x80) // Start at OUT_X_L with auto-increment

extern u8 rawData[6]; // Accessible from other files if needed
extern volatile bool acc_enabled;

extern volatile bool acc_enabled;