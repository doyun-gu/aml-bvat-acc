/**
  ******************************************************************************
  * @file    i2c_handler.c
  * @brief   This file provides code for I2C handling, specifically for an
  * LIS3DH accelerometer.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "i2c_handler.h" // Primary include for this module's definitions and dependencies
                         // This header should include:
                         // <stdio.h>, <string.h>, <stdbool.h>
                         // "stm32f4xx_hal.h", "aml_hal.h", "uart_handler.h"

/* Private define ------------------------------------------------------------*/
// No private defines needed here if all constants are in the header.

/* Private macro -------------------------------------------------------------*/
// No private macros needed here.

/* Private variables ---------------------------------------------------------*/
// These are the definitions for the variables declared 'extern' in i2c_handler.h
I2C_HandleTypeDef hi2c1;
volatile bool acc_enabled = false;
u8 rawData[6]; // Buffer for accelerometer data

/* Private function prototypes -----------------------------------------------*/
// Declare any static helper functions specific to this file here, if needed.

/* Exported functions --------------------------------------------------------*/

//------------------------------------------------------------------------------
// Initialization Functions
//------------------------------------------------------------------------------

/**
  * @brief  Initializes the I2C1 peripheral parameters and NVIC for interrupts.
  * @param  None
  * @retval None
  */
void MX_I2C1_Init(void) {
    // Using WriteUART_Blocking for critical init messages to avoid IT complexities
    // Ensure WriteUART_Blocking is prototyped and defined in uart_handler.c/h
    WriteUART_Blocking("Attempting HAL_I2C_Init...\r\n");

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000; // 100 kHz
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        WriteUART_Blocking("HAL_I2C_Init FAILED! Entering Error_Handler.\r\n");
        Error_Handler(); // Ensure Error_Handler() is defined (e.g., in aml_hal.c)
    }

    WriteUART_Blocking("HAL_I2C_Init Successful.\r\n");

    // NVIC configuration for I2C1 interrupts (Event and Error)
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
}

/**
  * @brief  Performs I2C setup for the application AFTER I2C1 peripheral is initialized.
  * Checks LIS3DH connectivity, and enables the accelerometer.
  * @param  None
  * @retval bool true if device setup was successful, false otherwise.
  */
bool BVAT_I2C_Init(void) {
    bool device_is_ok;

    // MX_I2C1_Init() is called once in AML_Init()
    WriteUART_Blocking("Performing I2C device checks (LIS3DH)...\r\n"); // Use blocking for reliability

    device_is_ok = I2C_connectivity_check(); // Call to the function defined below

    if (device_is_ok) {
        I2C_ACC_Enable(); // Call to the function defined below
        WriteUART_Blocking("LIS3DH Accelerometer configuration command sent.\r\n");
    } else {
        WriteUART_Blocking("LIS3DH Accelerometer NOT configured due to I2C issue.\r\n");
    }
    return device_is_ok;
}

//------------------------------------------------------------------------------
// I2C Operational Functions (LIS3DH Specific) - DEFINITIONS
//------------------------------------------------------------------------------

/**
  * @brief  Checks the connectivity of the LIS3DH accelerometer.
  * @note   Prints detailed status messages via UART.
  * @param  None
  * @retval bool true if the LIS3DH responds, false otherwise.
  */
bool I2C_connectivity_check(void) {
    HAL_StatusTypeDef status;
    uint32_t trials = 3;
    uint32_t timeout = 100; // ms

    static char msg_buffer[64]; // Static for IT-based WriteUART, or if WriteUART_Blocking uses it.

    // ACC_I2C_ADDR is defined in i2c_handler.h as (LIS3DH_7BIT_ADDR << 1)
    status = HAL_I2C_IsDeviceReady(&hi2c1, ACC_I2C_ADDR, trials, timeout);

    if (status == HAL_OK) {
        WriteUART_Blocking("I2C LIS3DH (0x18) is ready.\r\n");
        return true;
    } else {
        switch (status) {
            case HAL_ERROR:
                sprintf(msg_buffer, "I2C LIS3DH (0x18) error. Status: HAL_ERROR\r\n");
                break;
            case HAL_BUSY:
                sprintf(msg_buffer, "I2C LIS3DH (0x18) busy. Status: HAL_BUSY\r\n");
                break;
            case HAL_TIMEOUT:
                sprintf(msg_buffer, "I2C LIS3DH (0x18) timeout. Status: HAL_TIMEOUT\r\n");
                break;
            default:
                sprintf(msg_buffer, "I2C LIS3DH (0x18) not found. Status: %d\r\n", status);
                break;
        }
        WriteUART_Blocking(msg_buffer);
        return false;
    }
}

/**
 * @brief  Enables the LIS3DH accelerometer by writing to its CTRL_REG1.
 * @note   Uses interrupt-driven I2C write.
 * ACC_CTRL_REG1_CONFIG_100HZ and LIS3DH_REG_CTRL_REG1 are defined in i2c_handler.h.
 * @param  None
 * @retval None
 */
// In i2c_handler.c
void I2C_ACC_Enable(void) {
    u8 ctrl1_config_value = ACC_CTRL_REG1_CONFIG_100HZ; // Should be 0x57
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Write(&hi2c1,
                               ACC_I2C_ADDR,
                               LIS3DH_REG_CTRL_REG1, // Target register (0x20)
                               I2C_MEMADD_SIZE_8BIT,
                               &ctrl1_config_value,  // Data to write (0x57)
                               1,                    // Size of data (1 byte)
                               100);                 // Timeout in ms (e.g., 100ms)

    if (status == HAL_OK) {
        WriteUART_Blocking("LIS3DH CTRL_REG1 Write OK (Blocking).\r\n"); // Use blocking UART for this test
        acc_enabled = true; // Set your global flag
        
    } else {
        static char err_buf[64]; // static if WriteUART_Blocking might use IT internally
        sprintf(err_buf, "LIS3DH CTRL_REG1 Write FAILED (Blocking). Status: %d\r\n", status);
        WriteUART_Blocking(err_buf);
        acc_enabled = false;
    }
}

/**
 * @brief  Initiates an interrupt-driven read of 6 bytes (X,Y,Z data) from LIS3DH.
 * @note   Data is read into the global 'rawData' buffer. Completion is signaled
 * via HAL_I2C_MemRxCpltCallback. LIS3DH_DATA_START_MULTI_READ is defined
 * in i2c_handler.h.
 * @param  None
 * @retval None
 */
void I2C_Read_ACC(void) {
    HAL_I2C_Mem_Read_IT(&hi2c1,
                        ACC_I2C_ADDR,
                        LIS3DH_DATA_START_MULTI_READ,
                        I2C_MEMADD_SIZE_8BIT,
                        rawData,  // Global buffer
                        6);       // Read 6 bytes
}

/**
 * @brief  Reads a single byte from a specified register of the LIS3DH in blocking mode.
 * @param  reg: The 8-bit LIS3DH register address to read from.
 * @retval u8: The value read from the register.
 */
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
        static char err_buf[64]; // Static for safety if WriteUART_Blocking is IT-based
        sprintf(err_buf, "Error reading I2C reg 0x%02X. Status: %d\r\n", reg, status);
        WriteUART_Blocking(err_buf);
    }
    return value;
}

/**
 * @brief  Test function to read LIS3DH accelerometer data in blocking mode and print via UART.
 * @note   LIS3DH_DATA_START_MULTI_READ is defined in i2c_handler.h.
 * @param  None
 * @retval None
 */
void read_I2C_Blocking (void) { // Definition matches prototype now
    u8 data[6];
    HAL_StatusTypeDef status;
    static char temp_buffer[128]; // Static for safety with IT-based WriteUART, ensure size

    status = HAL_I2C_Mem_Read(&hi2c1,
                              ACC_I2C_ADDR,
                              LIS3DH_DATA_START_MULTI_READ,
                              I2C_MEMADD_SIZE_8BIT,
                              data,
                              sizeof(data),
                              1000);
    
    if (status == HAL_OK) {
        sprintf(temp_buffer, "I2C Block Read OK: XL=%02X XH=%02X YL=%02X YH=%02X ZL=%02X ZH=%02X\r\n",
                data[0], data[1], data[2], data[3], data[4], data[5]);
        WriteUART_Blocking(temp_buffer);
    } else {
        sprintf(temp_buffer, "I2C Block Read Failed. Status: %d\r\n", status);
        WriteUART_Blocking(temp_buffer);
    }
}

/* HAL I2C Callbacks ---------------------------------------------------------*/

/**
  * @brief  Memory Tx Transfer completed callback.
  * @param  hi2c: I2C handle pointer.
  * @retval None
  */
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C1) {
        acc_enabled = true;
        WriteUART_Blocking("LIS3DH CTRL_REG1 Write Complete (IT).\r\n"); // Use blocking for callback debug
    }
}

/**
  * @brief  Memory Rx Transfer completed callback.
  * @param  hi2c: I2C handle pointer.
  * @retval None
  */
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1) {
        static char temp_buffer[128];
        sprintf(temp_buffer, "I2C IT Read OK: XL=%02X XH=%02X YL=%02X YH=%02X ZL=%02X ZH=%02X\r\n",
                rawData[0], rawData[1], rawData[2], rawData[3], rawData[4], rawData[5]);
        WriteUART_Blocking(temp_buffer); // Use blocking for callback debug
    }
}

/**
  * @brief  I2C error callback.
  * @param  hi2c: I2C handle pointer.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c->Instance == I2C1) {
        static char err_buf[64];
        sprintf(err_buf, "I2C Error Occurred. Code: 0x%08lX\r\n", hi2c->ErrorCode);
        WriteUART_Blocking(err_buf); // Use blocking for callback debug
    }
}

/**
 * @brief Reads the raw accelerometer data and converts it to 'g's.
 * @param x_g: Pointer to a float where the X-axis acceleration in 'g' will be stored.
 * @param y_g: Pointer to a float where the Y-axis acceleration in 'g' will be stored.
 * @param z_g: Pointer to a float where the Z-axis acceleration in 'g' will be stored.
 * @retval bool: true on successful read, false on failure.
 */
bool LIS3DH_Read_Accel_Data(float* x_g, float* y_g, float* z_g)
{
    u8 data[6];
    HAL_StatusTypeDef status;

    // Read the 6 bytes of accelerometer data
    status = HAL_I2C_Mem_Read(&hi2c1,
                              ACC_I2C_ADDR,
                              LIS3DH_DATA_START_MULTI_READ,
                              I2C_MEMADD_SIZE_8BIT,
                              data,
                              6,
                              1000);

    if (status != HAL_OK) {
        WriteUART_Blocking("I2C Accelerometer Read Failed.\r\n");
        return false;
    }

    // Combine LSB and MSB, right-shift for 12-bit resolution, and scale
    int16_t raw_x = (int16_t)((data[1] << 8) | data[0]) >> 4;
    int16_t raw_y = (int16_t)((data[3] << 8) | data[2]) >> 4;
    int16_t raw_z = (int16_t)((data[5] << 8) | data[4]) >> 4;

    // Convert raw values to 'g'.
    // Sensitivity for LIS3DH in HR mode at +/-2g is 1 mg/LSB. 1000 mg = 1 g.
    *x_g = (float)raw_x / 1000.0f;
    *y_g = (float)raw_y / 1000.0f;
    *z_g = (float)raw_z / 1000.0f;

    return true;
}