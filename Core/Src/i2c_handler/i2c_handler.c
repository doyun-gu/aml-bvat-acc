#include "i2c_handler.h"

u8 accelerometerADDR = 0x18 << 1;
u8 regADDR = 0x20;
u8 regValue;

u8 sensor_data[6];
u8 sensorADDR = 0x19 << 1;
u8 registerADDR = 0x28;

u8 sensordata_ready = 0;

u8 regValue = 0; // Global or static buffer to store read value

u8 readACC(u8 regADDR) {
    u8 data = 0;
    u8 addr = 0x18 << 1;

    HAL_I2C_Mem_Read(&hi2c1,
                     addr,
                     regADDR,
                     I2C_MEMADD_SIZE_8BIT,
                     &data,
                     1,
                     HAL_MAX_DELAY);
    return data;
}

//* Will be called when the I2C memory read operation is complete
//* This callback is triggered when the I2C memory read operation is complete
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1) {
        // Process `sensor_data` now
    }
}