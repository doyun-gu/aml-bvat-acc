/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
#include "uart_handler.h" // For access to HAL handles and WriteUART_Blocking
#include "i2c_handler.h"  // For access to I2C handle and functions
/* External variables --------------------------------------------------------*/
// Handles are now externed via uart_handler.h

/******************************************************************************/
/* Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
// ... (NMI_Handler, HardFault_Handler, etc. remain the same) ...
void NMI_Handler(void) { while (1) {} }
void HardFault_Handler(void) { while (1) {} }
void MemManage_Handler(void) { while (1) {} }
void BusFault_Handler(void) { while (1) {} }
void UsageFault_Handler(void) { while (1) {} }
void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/******************************************************************************/

/**
  * @brief This function handles DMA2 stream5 global interrupt.
  * @note  This is the interrupt for our USART1_RX DMA (GPS data).
  */
void DMA2_Stream5_IRQHandler(void)
{
  // --- ADDED DIAGNOSTIC LINE ---
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  // This message will print ONLY if the DMA hardware interrupt is successfully triggered.
  // WriteUART_Blocking("[DEBUG]: DMA2_Stream5_IRQHandler Entered!\r\n");
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart2);
}


/**
  * @brief This function handles I2C1 event interrupt.
  */
void I2C1_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&hi2c1);
}

/**
  * @brief This function handles I2C1 error interrupt.
  */
void I2C1_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&hi2c1);
}