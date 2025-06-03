#include "uart_handler.h"

// TODO1: UART is not working - either blocking or non-blocking mode
// // TODO2: Check if the UART is working with the blocking mode
// TODO3: Check if the UART is working with the non-blocking mode
// TODO4: Check if the UART is working with the interrupt mode

UART_HandleTypeDef huart2;

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART2_UART_Init(void)
{
  // 1. Assign the specific hardware instance (USART2) to the handle.
  huart2.Instance = USART2;

  // 2. Configure the basic communication parameters:
  huart2.Init.BaudRate = 115200;          // Sets baud rate to 115200 bps
  huart2.Init.WordLength = UART_WORDLENGTH_8B; // 8 data bits
  huart2.Init.StopBits = UART_STOPBITS_1;   // 1 stop bit
  huart2.Init.Parity = UART_PARITY_NONE;    // No parity bit
  huart2.Init.Mode = UART_MODE_TX_RX;       // Enable both Transmit and Receive
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE; // No hardware flow control (RTS/CTS)
  huart2.Init.OverSampling = UART_OVERSAMPLING_16; // Standard oversampling

  // 3. Call the HAL function to apply these settings to the hardware.
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    // 4. If initialization fails, call the error handler.
    Error_Handler();
  }
}

/**
 * ! Tester Functions for UART =============================================
 * ! These functions are used to send messages over UART.
 * 
 * *1. Blocking Mode UART
 * *2. Non-blocking Mode UART
 * *3. Interrupt Mode UART
 */

/**
  * @brief  Transmits a message over UART in blocking mode.
  * @param  message: Pointer to the null-terminated string to be transmitted.
  * @retval None
  */
void WriteUART_Blocking(const char *message) // Renamed for clarity, or you can keep WriteBUART
{
    if (message == NULL) {
        return; // Or handle error
    }

    uint16_t message_len = strlen(message);
    if (message_len == 0) {
        return; // Nothing to send
    }

    // Use HAL_UART_Transmit for blocking mode
    // It will wait until all data is sent or the timeout expires.
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2,        // Your UART handle
                                                (uint8_t *)message, // Pointer to data buffer
                                                message_len,        // Length of data
                                                HAL_MAX_DELAY);     // Timeout (HAL_MAX_DELAY waits indefinitely)
                                                                    // You can use a specific timeout like 1000 (ms)

    if (status != HAL_OK)
    {
        // Handle transmission error, e.g., by calling Error_Handler()
        // Or by setting an error flag, or logging.
        // For example:
        // Error_Handler();
    }
}

/**
  * @brief  Transmits a null-terminated string message over UART using interrupt-driven (non-blocking) mode.
  * @param  message: Pointer to the null-terminated string to be transmitted.
  * @retval HAL_StatusTypeDef: Status of the UART transmission initiation.
  * - HAL_OK: Transmission was successfully initiated.
  * - HAL_ERROR: Parameter error (e.g., null message) or other HAL error.
  * - HAL_BUSY: UART is busy with a previous transmission (if using IT or DMA).
  * The caller might need to wait or queue the message.
  */
HAL_StatusTypeDef WriteUART(const char *message) {
    HAL_StatusTypeDef status = HAL_OK;

    // 1. Check for NULL pointer
    if (message == NULL) {
        // Optionally, log this error or handle it in a specific way
        // For example, you could transmit a predefined error string, or do nothing.
        return HAL_ERROR; // Indicate a parameter error
    }

    // 2. Calculate the length of the message
    uint16_t message_len = strlen(message);

    // 3. Check for zero-length string (optional, but good practice)
    if (message_len == 0) {
        // Nothing to send, so we can consider it successful or an error depending on requirements.
        // For now, let's say it's okay, as no transmission is needed.
        return HAL_OK;
    }

    // 4. Check if UART is busy (Optional but good for robust IT transmissions)
    //    If a previous IT transmission is ongoing, HAL_UART_Transmit_IT will return HAL_BUSY.
    //    You might want to handle this explicitly before calling.
    if (HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_TX ||
        HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_TX_RX) {
        // UART is currently busy with another transmission.
        // What to do?
        // Option A: Return HAL_BUSY and let the caller handle it (e.g., retry later, queue message). This is common.
        // Option B: Wait for a short period (making it somewhat blocking, use with caution).
        // Option C: Implement a transmit queue (more complex).
        return HAL_BUSY; // Let the caller know the UART is busy.
    }

    // 5. Initiate the interrupt-driven transmission
    //    The cast to (uint8_t*) is because HAL_UART_Transmit_IT expects a byte pointer.
    //    Your 'u8' type from aml_hal.h is likely uint8_t, so (u8*) is also fine.
    status = HAL_UART_Transmit_IT(&huart2, (uint8_t *)message, message_len);

    // 6. Handle potential errors from initiating the transmission
    if (status != HAL_OK) {
        // An error occurred while trying to START the transmission (e.g., HAL_BUSY if not checked above, or HAL_ERROR).
        // You might want to log this, toggle an error LED, or call Error_Handler()
        // if failing to initiate a UART message is a critical failure.
        // For example:
        // Error_Handler();
    }

    return status; // Return the status of HAL_UART_Transmit_IT
}

//! No need for now
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//     if (huart->Instance == USART2) {
//         // Handle received data here
//         // For example, you can read the received data from huart->pRxBuffPtr
//         // and process it as needed.
//     }
// }

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        // Handle transmission complete here
        // For example, you can set a flag or perform other actions after transmission.
    }
}