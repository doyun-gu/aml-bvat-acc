/**
  ******************************************************************************
  * @file    command_handler.c
  * @brief   This file handles high-level command logic, timing for measurements,
  * and triggering data acquisition.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "command_handler.h"
// Other includes like i2c_handler.h, timer.h, uart_handler.h are brought in via command_handler.h

/* Private define ------------------------------------------------------------*/
#define MEASUREMENT_INTERVAL_MS 1000 // Interval for triggering accelerometer reads

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static u32 s_command_timer = 0;         ///< Timer for periodic command/measurement execution.
static bool s_i2c_module_ready = false; ///< Flag indicating if I2C peripheral and target device are ready.

// The global 'acc_enabled' (from i2c_handler.h) indicates if the LIS3DH has been successfully configured.
// The global 'rawData' (from i2c_handler.h) is where LIS3DH data is stored by I2C_Read_ACC.

/* Private function prototypes -----------------------------------------------*/
// TODO: SD Card needs to be discussed -> Not available with the current setup
//* What would be better to log data to SD card or just UART?

// static void log_to_sd(u32 timestamp, int16_t acc_x, int16_t acc_y, int16_t acc_z) {
//     FIL file;
//     UINT bytes_written;
//     char buffer[128]; // Increased buffer size for potentially longer strings

//     if (f_open(&file, "data.csv", FA_OPEN_APPEND | FA_WRITE) != FR_OK) {
//         WriteUART_Blocking("SD file open failed.\r\n"); // Assuming WriteUART_Blocking for critical messages
//         return;
//     }

//     // Example for logging processed 16-bit values
//     snprintf(buffer, sizeof(buffer), "%lu,%d,%d,%d\r\n", timestamp, acc_x, acc_y, acc_z);
//     if (f_write(&file, buffer, strlen(buffer), &bytes_written) != FR_OK || bytes_written == 0) {
//         WriteUART_Blocking("SD file write failed.\r\n");
//     }

//     f_close(&file);
// }


/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initializes command handler specific components (e.g., timers).
 * @param None
 * @retval None
 */
void CommandHandler_Init(void) {
    s_command_timer = HAL_GetTick();      // Initialize command timer
    // Any other command_handler specific initializations can go here.
}

/**
 * @brief Sets the status of I2C module readiness.
 * @note This should be called from main after BVAT_I2C_Init() has determined
 * the I2C system and target device status.
 * @param is_ready: true if the I2C peripheral and target device are initialized and ready, false otherwise.
 * @retval None
 */
void CommandHandler_SetI2CReady(bool is_ready) {
    s_i2c_module_ready = is_ready;
}

/**
 * @brief Main measurement loop logic.
 * Checks I2C and accelerometer status, and periodically triggers an accelerometer data read.
 * Actual data processing and UART logging of accelerometer data is expected to occur
 * within I2C HAL callbacks (e.g., HAL_I2C_MemRxCpltCallback in i2c_handler.c).
 * @param None
 * @retval None
 */
void measure_loop(void) {
    if (!s_i2c_module_ready) {
        // This message might be too frequent if I2C init failed.
        // Consider printing it once or less often from main loop.
        // WriteUART_Blocking("I2C system not ready, skipping measurement.\r\n");
        HAL_Delay(MEASUREMENT_INTERVAL_MS); // Still delay to keep a rhythm
        return;
    }

    // acc_enabled is a volatile bool externed from i2c_handler.h
    // It should be set to true in HAL_I2C_MemTxCpltCallback after I2C_ACC_Enable's IT write completes.
    if (!acc_enabled) {
        WriteUART_Blocking("Accelerometer not enabled, attempting to enable...\r\n");
        I2C_ACC_Enable(); // This function uses IT write, acc_enabled will be set in its callback.
                          // We might not read immediately after this call in the same loop iteration.
    }

    // Check timer for periodic measurement trigger
    if (timer_handler(MEASUREMENT_INTERVAL_MS, &s_command_timer)) {
        if (s_i2c_module_ready && acc_enabled) {
            // WriteUART_Blocking("Triggering I2C Accelerometer Read (IT)...\r\n"); // Optional debug
            I2C_Read_ACC(); // Initiate an interrupt-driven read into global rawData.
                            // The result will be processed and printed in HAL_I2C_MemRxCpltCallback
                            // (defined in i2c_handler.c).
        } else if (s_i2c_module_ready && !acc_enabled) {
            WriteUART_Blocking("Measurement interval: Accelerometer not yet enabled by IT callback.\r\n");
        }
    }
}