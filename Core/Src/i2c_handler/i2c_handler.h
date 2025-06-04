#pragma once

//==============================================================================
// Includes
//==============================================================================
// Standard C Includes
#include <stdio.h>       // For printf and other standard I/O functions
#include <string.h>      // For string manipulation functions like strlen, strcpy, etc.
#include <stdbool.h>     // For bool, true, false data types


// STM32 HAL Includes
#include "stm32f4xx_hal.h" // For I2C_HandleTypeDef and core HAL types

// Custom Project HAL/Type Includes
#include "aml_hal.h"       // For custom types like u8

// Other Module Includes
// Note: "uart_handler.h" is included here as per your original file.
// If only functions in i2c_handler.c use WriteUART, it's often cleaner
// for i2c_handler.c to include uart_handler.h directly.
#include "uart_handler.h"

//==============================================================================
// LIS3DH Accelerometer Specific Constants
//==============================================================================
#define LIS3DH_7BIT_ADDR              (0x18)        // LIS3DH default 7-bit address (SA0 = GND)
#define ACC_I2C_ADDR                  (LIS3DH_7BIT_ADDR << 1) // 8-bit address for HAL functions

// --- LIS3DH Register Definitions ---
#define LIS3DH_REG_WHO_AM_I           (0x0F)        // WHO_AM_I device identification register
#define LIS3DH_WHO_AM_I_EXPECTED_VAL  (0x33)        // Expected value from WHO_AM_I register

#define LIS3DH_REG_CTRL_REG1          (0x20)        // Control Register 1 (ODR, LPen, Axes enable)
#define LIS3DH_REG_CTRL_REG4          (0x23)        // Control Register 4 (BDU, FS, HR)

#define LIS3DH_REG_OUT_X_L            (0x28)        // X-axis LSB output data register
// Use this for multi-byte reads starting from OUT_X_L with auto-increment
#define LIS3DH_DATA_START_MULTI_READ  (LIS3DH_REG_OUT_X_L | 0x80)

// --- LIS3DH Configuration Values ---
// Example: 100Hz ODR, Normal mode, X/Y/Z axes enabled
#define ACC_CTRL_REG1_CONFIG_100HZ    (0x57)        // (0b01010111)
// Example: Block Data Update, +/-2g Full Scale, High Resolution enabled
#define ACC_CTRL_REG4_CONFIG_DEFAULT  (0x88)        // (0b10001000)


//==============================================================================
// Extern Variable Declarations
// (Variables defined in i2c_handler.c and accessible globally)
//==============================================================================
extern I2C_HandleTypeDef hi2c1;         ///< I2C1 Handle structure.
extern volatile bool acc_enabled;       ///< Flag indicating if the accelerometer has been successfully enabled.
extern u8 rawData[6];                  ///< Buffer to store raw accelerometer data [X_L, X_H, Y_L, Y_H, Z_L, Z_H].


//==============================================================================
// Function Prototypes
//==============================================================================

//------------------------------------------------------------------------------
// Initialization Functions
//------------------------------------------------------------------------------
/**
 * @brief  Initializes the I2C1 peripheral parameters.
 * @note   This function calls HAL_I2C_Init() and relies on HAL_I2C_MspInit()
 * for low-level clock and GPIO configuration. NVIC for I2C interrupts
 * is also typically configured here or in MspInit.
 * @param  None
 * @retval None
 */
void MX_I2C1_Init(void);

/**
 * @brief  Performs the complete I2C setup for the application.
 * Initializes I2C1, checks LIS3DH connectivity, and enables the accelerometer.
 * @param  None
 * @retval bool true if initialization and device setup were successful, false otherwise.
 */
bool BVAT_I2C_Init(void);

//------------------------------------------------------------------------------
// I2C Operational Functions (LIS3DH Specific)
//------------------------------------------------------------------------------
/**
 * @brief  Checks connectivity with the LIS3DH accelerometer using HAL_I2C_IsDeviceReady.
 * @note   Prints detailed status messages via UART.
 * @param  None
 * @retval bool true if the LIS3DH responds, false otherwise.
 */
bool I2C_connectivity_check(void);

/**
 * @brief  Enables the LIS3DH accelerometer.
 * @note   Writes to LIS3DH control registers, likely using interrupt-driven I2C.
 * @param  None
 * @retval None
 */
void I2C_ACC_Enable(void);

/**
 * @brief  Initiates an interrupt-driven read of 6 bytes (X,Y,Z data) from LIS3DH.
 * @note   Data is read into the global 'rawData' buffer.
 * Completion is signaled via HAL_I2C_MemRxCpltCallback.
 * @param  None
 * @retval None
 */
void I2C_Read_ACC(void);

/**
 * @brief  Reads a single byte from a specified register of the LIS3DH in blocking mode.
 * @param  reg: The 8-bit LIS3DH register address to read from.
 * @retval u8: The value read from the register.
 */
// u8 readACC(u8 reg);

/**
 * @brief I2C Blocking Mode Read
 * @param None
 * @retval None
 */
void read_I2C_Blocking(void);

//------------------------------------------------------------------------------
// HAL I2C Callback Function Prototypes (Implemented in i2c_handler.c)
//------------------------------------------------------------------------------
/**
 * @brief  I2C Master Transmit completed callback.
 * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
 * the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);

/**
 * @brief  I2C Master Receive completed callback.
 * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
 * the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);

/**
 * @brief  I2C Error callback.
 * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
 * the configuration information for the specified I2C.
 * @retval None
 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);