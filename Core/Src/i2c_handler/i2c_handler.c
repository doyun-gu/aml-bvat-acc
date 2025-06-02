#include "i2c_handler.h"

I2C_HandleTypeDef hi2c1;

volatile bool acc_enabled = false;
u8 rawData[6];

void MX_I2C1_Init(void) {
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
        Error_Handler();
    }

    // Enable and set I2C1 Event Interrupt to priority 0
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);

    // Enable and set I2C1 Error Interrupt to priority 1
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
}

// void I2C_connectivity_check (void) {
//     HAL_StatusTypeDef status;
//     u8 deviceADDR = 0x18 << 1;      // LIS3DH address shifted

//     status = HAL_I2C_IsDeviceReady(&hi2c1, deviceADDR, 3, 1000);

//     if (status == HAL_OK) {
//         WriteUART("I2C device is ready.\n");
//     } else {
//         WriteUART("I2C device not found.\n");
//     }
// }

bool I2C_connectivity_check (void) {
    HAL_StatusTypeDef status;
    u8 deviceADDR = 0x18 << 1; // LIS3DH address shifted

    status = HAL_I2C_IsDeviceReady(&hi2c1, deviceADDR, 3, 1000);

    if (status == HAL_OK) {
        WriteUART("I2C device is ready.\n");
        return true; // Device is ready
    } else {
        WriteUART("I2C device not found.\n");
        return false; // Device not found
    }
}

/**
 * Accelerometer I2C address and register definitions
 */

 // TODO: Need to work with I2C Mem Read/Write with I2C configurations
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

u8 readACC(u8 reg) {
    u8 value = 0;
    HAL_I2C_Mem_Read(&hi2c1, ACC_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, HAL_MAX_DELAY);
    return value;
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

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1) {
        WriteUART("I2C error occurred.\n");
        // Optional: Try reinitializing or setting error flags
    }
}