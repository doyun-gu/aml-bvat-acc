#include "i2c_handler.h"

volatile bool acc_enabled = false;

/**
 * Accelerometer I2C address and register definitions
 */

void I2C_ACC_Enable(void) {
    static u8 ctrl1 = ACC_CTRL1_VALUE;

    HAL_I2C_Mem_Write_IT(&hi2c1,
                         ACC_I2C_ADDR,
                         ACC_CTRL1_REG,
                         I2C_MEMADD_SIZE_8BIT,
                         &ctrl1,
                         1);
}

void I2C_Read_ACC(void) {
    HAL_I2C_Mem_Read_IT(&hi2c1,
                        ACC_I2C_ADDR,
                        ACC_DATA_START,
                        I2C_MEMADD_SIZE_8BIT,
                        rawData,
                        6);
}

// Write complete callback
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
        // acc_enabled = true;
    }
}

//* Will be called when the I2C memory read operation is complete
//* This callback is triggered when the I2C memory read operation is complete
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1) {
        // Process `sensor_data` now
    }
}