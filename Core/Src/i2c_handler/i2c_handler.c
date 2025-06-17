/**
  ******************************************************************************
  * @file    i2c_handler.c
  * @brief   This file provides code for I2C handling, specifically for an
  *          LIS3DH accelerometer.
  ******************************************************************************
  */

#include "i2c_handler.h"

I2C_HandleTypeDef hi2c1;
volatile bool acc_enabled = false;
u8 rawData[6];

void MX_I2C1_Init(void) {
    WriteUART_Blocking("Attempting HAL_I2C_Init...\r\n");

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        WriteUART_Blocking("HAL_I2C_Init FAILED! Entering Error_Handler.\r\n");
        Error_Handler();
    }

    WriteUART_Blocking("HAL_I2C_Init Successful.\r\n");

    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
}

bool BVAT_I2C_Init(void) {
    bool device_is_ok;
    WriteUART_Blocking("Performing I2C device checks (LIS3DH)...\r\n");
    device_is_ok = I2C_connectivity_check();

    if (device_is_ok) {
        I2C_ACC_Enable();
        HAL_Delay(100);
        WriteUART_Blocking("LIS3DH Accelerometer configuration command sent.\r\n");
    } else {
        WriteUART_Blocking("LIS3DH Accelerometer NOT configured due to I2C issue.\r\n");
    }
    return device_is_ok;
}

bool I2C_connectivity_check(void) {
    HAL_StatusTypeDef status;
    uint32_t trials = 3;
    uint32_t timeout = 100;
    static char msg_buffer[64];

    status = HAL_I2C_IsDeviceReady(&hi2c1, ACC_I2C_ADDR, trials, timeout);

    if (status == HAL_OK) {
        WriteUART_Blocking("I2C LIS3DH (0x18) is ready.\r\n");
        return true;
    } else {
        sprintf(msg_buffer, "I2C LIS3DH (0x18) not found. Status: %d\r\n", status);
        WriteUART_Blocking(msg_buffer);
        return false;
    }
}

void I2C_ACC_Enable(void) {
    u8 ctrl1_config_value = ACC_CTRL_REG1_CONFIG_100HZ;
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Write(&hi2c1,
                               ACC_I2C_ADDR,
                               LIS3DH_REG_CTRL_REG1,
                               I2C_MEMADD_SIZE_8BIT,
                               &ctrl1_config_value,
                               1,
                               100);

    if (status == HAL_OK) {
        WriteUART_Blocking("LIS3DH CTRL_REG1 Write OK (Blocking).\r\n");
        acc_enabled = true;
    } else {
        static char err_buf[64];
        sprintf(err_buf, "LIS3DH CTRL_REG1 Write FAILED. Status: %d\r\n", status);
        WriteUART_Blocking(err_buf);
        acc_enabled = false;
    }
}

u8 readACC(u8 reg) {
    u8 value = 0;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1,
                                               ACC_I2C_ADDR,
                                               reg,
                                               I2C_MEMADD_SIZE_8BIT,
                                               &value,
                                               1,
                                               HAL_MAX_DELAY);
    if (status != HAL_OK) {
        static char err_buf[64];
        sprintf(err_buf, "Error reading I2C reg 0x%02X. Status: %d\r\n", reg, status);
        WriteUART_Blocking(err_buf);
    }
    return value;
}

bool LIS3DH_Read_Accel_Data(float* x_g, float* y_g, float* z_g) {
    u8 data[6];
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Read(&hi2c1,
                              ACC_I2C_ADDR,
                              LIS3DH_DATA_START_MULTI_READ,
                              I2C_MEMADD_SIZE_8BIT,
                              data,
                              6,
                              1000);

    if (status != HAL_OK) {
        char err_msg[64];
        sprintf(err_msg, "LIS3DH I2C Read FAILED. Status: %d\r\n", status);
        WriteUART_Blocking(err_msg);
        return false;
    }

    int16_t raw_x = (int16_t)((data[1] << 8) | data[0]) >> 4;
    int16_t raw_y = (int16_t)((data[3] << 8) | data[2]) >> 4;
    int16_t raw_z = (int16_t)((data[5] << 8) | data[4]) >> 4;

    *x_g = (float)raw_x / 1000.0f;
    *y_g = (float)raw_y / 1000.0f;
    *z_g = (float)raw_z / 1000.0f;

    // static char float_buf[128];
    // sprintf(float_buf, "DEBUG RAW: AccelX=%.3f Y=%.3f Z=%.3f\r\n", *x_g, *y_g, *z_g);
    // WriteUART_Blocking(float_buf);

    return true;
}
