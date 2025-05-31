#include "i2c_handler.h"

u8 accelerometerADDR = 0x18 << 1;
u8 regADDR = 0x20;
u8 regValue;

u8 sensor_data[6];
u8 sensorADDR = 0x19 << 1;
u8 registerADDR = 0x28;

u8 sensordata_ready = 0;

// HAL_I2C_Mem_Read_IT(&hi2c1, SENSOR_I2C_ADDR, SENSOR_REG_ADDR, I2C_MEMADD_SIZE_8BIT, sensor_data, 6);

void read_sensor_data() {

    HAL_I2C_Mem_Read_IT(&hi2c1,
                            SENSOR_I2C_ADDR,
                            SENSOR_REG_ADDR,
                            I2C_MEMADD_SIZE_8BIT,
                            sensor_data,
                            6);
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1) {
        // Process `sensor_data` now
    }
}