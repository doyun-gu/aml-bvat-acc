#pragma once

//==============================================================================
// Standard C Includes
//==============================================================================
#include <stdint.h> // For standard integer types like uint8_t, uint16_t, etc.

//==============================================================================
// STM32 HAL Includes
//==============================================================================
#include "stm32f4xx_hal.h" // Core HAL types, GPIO_TypeDef, etc.

//==============================================================================
// Custom Data Type Definitions for AML (Aston Martin Lagonda) HAL
//==============================================================================
typedef uint8_t  u8;     ///< Unsigned 8-bit integer.
typedef uint16_t u16;    ///< Unsigned 16-bit integer.
typedef uint32_t u32;    ///< Unsigned 32-bit integer.
typedef int8_t   s8;     ///< Signed 8-bit integer.
typedef int16_t  s16;    ///< Signed 16-bit integer.
typedef int32_t  s32;    ///< Signed 32-bit integer.
// Consider using standard <stdbool.h> for bool, true, false if not already globally available
// For now, keeping your bool_t for consistency if used elsewhere.
typedef uint8_t  bool_t; ///< Boolean type (0 or 1).

//==============================================================================
// Pin Structure Definition
//==============================================================================
/**
 * @brief Structure to hold a pin's port and number.
 */
typedef struct {
    GPIO_TypeDef *port; ///< Pointer to the GPIO port (e.g., GPIOA, GPIOB).
    u16 pin;            ///< The GPIO pin number (e.g., GPIO_PIN_5).
} Pin_t; // Changed name to Pin_t to avoid potential conflict with a 'Pin' macro if any

//==============================================================================
// STM32F411RE Nucleo Board Pin Definitions (Arduino Style)
//==============================================================================
// --- Digital Pins (D0-D15) ---
#define D0_PORT     GPIOA
#define D0_PIN      GPIO_PIN_3
#define D0          (Pin_t){D0_PORT, D0_PIN}

#define D1_PORT     GPIOA
#define D1_PIN      GPIO_PIN_2
#define D1          (Pin_t){D1_PORT, D1_PIN}

#define D2_PORT     GPIOA
#define D2_PIN      GPIO_PIN_10
#define D2          (Pin_t){D2_PORT, D2_PIN}

#define D3_PORT     GPIOB
#define D3_PIN      GPIO_PIN_3
#define D3          (Pin_t){D3_PORT, D3_PIN}

#define D4_PORT     GPIOB
#define D4_PIN      GPIO_PIN_5
#define D4          (Pin_t){D4_PORT, D4_PIN}

#define D5_PORT     GPIOB
#define D5_PIN      GPIO_PIN_4
#define D5          (Pin_t){D5_PORT, D5_PIN}

#define D6_PORT     GPIOB
#define D6_PIN      GPIO_PIN_10
#define D6          (Pin_t){D6_PORT, D6_PIN}

#define D7_PORT     GPIOA                     // Corrected from GPIOB based on common Nucleo-F411RE (e.g. for SPI MOSI)
#define D7_PIN      GPIO_PIN_8                // Often PA8, check your specific Nucleo variant if D7 is critical
#define D7          (Pin_t){D7_PORT, D7_PIN}  // If D7 is actually PB8, it conflicts with I2C SCL. Please verify.

#define D8_PORT     GPIOA
#define D8_PIN      GPIO_PIN_9
#define D8          (Pin_t){D8_PORT, D8_PIN}

#define D9_PORT     GPIOC
#define D9_PIN      GPIO_PIN_7
#define D9          (Pin_t){D9_PORT, D9_PIN}

#define D10_PORT    GPIOB
#define D10_PIN     GPIO_PIN_6
#define D10         (Pin_t){D10_PORT, D10_PIN}

#define D11_PORT    GPIOA
#define D11_PIN     GPIO_PIN_7
#define D11         (Pin_t){D11_PORT, D11_PIN}

#define D12_PORT    GPIOA
#define D12_PIN     GPIO_PIN_6
#define D12         (Pin_t){D12_PORT, D12_PIN}

#define D13_PORT    GPIOA           // User LED (LD2) on Nucleo-F411RE
#define D13_PIN     GPIO_PIN_5
#define D13         (Pin_t){D13_PORT, D13_PIN}

#define D14_PORT    GPIOB           // I2C1_SDA on Nucleo-F411RE (Arduino A4/D14)
#define D14_PIN     GPIO_PIN_9
#define D14         (Pin_t){D14_PORT, D14_PIN}

#define D15_PORT    GPIOB           // I2C1_SCL on Nucleo-F411RE (Arduino A5/D15)
#define D15_PIN     GPIO_PIN_8
#define D15         (Pin_t){D15_PORT, D15_PIN}

// --- Analog Pins (A0-A5) ---
// These can also be used as digital I/O
#define A0_PORT     GPIOA
#define A0_PIN      GPIO_PIN_0
#define A0          (Pin_t){A0_PORT, A0_PIN}

#define A1_PORT     GPIOA
#define A1_PIN      GPIO_PIN_1
#define A1          (Pin_t){A1_PORT, A1_PIN}

#define A2_PORT     GPIOA
#define A2_PIN      GPIO_PIN_4
#define A2          (Pin_t){A2_PORT, A2_PIN}

#define A3_PORT     GPIOB
#define A3_PIN      GPIO_PIN_0
#define A3          (Pin_t){A3_PORT, A3_PIN}

#define A4_PORT     GPIOC
#define A4_PIN      GPIO_PIN_1
#define A4          (Pin_t){A4_PORT, A4_PIN}

#define A5_PORT     GPIOC
#define A5_PIN      GPIO_PIN_0
#define A5          (Pin_t){A5_PORT, A5_PIN}

//==============================================================================
// Function Prototypes for aml_hal.c
//==============================================================================

/**
 * @brief  Initializes core HAL, system clocks, and base peripherals.
 * @note   This function should be called once at the beginning of main().
 * It calls HAL_Init(), SystemClock_Config(), and various MX_..._Init() functions.
 * @param  None
 * @retval None
 */
void AML_Init(void);

/**
  * @brief  This function is executed in case of error occurrence.
  * @note   Halts the system in an infinite loop after disabling interrupts.
  * Attempts to turn on g_builtinLed if initialized.
  * @param  None
  * @retval None
  */
void Error_Handler(void);

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  * where the assert_param error has occurred.
  * @param  file Pointer to the source file name.
  * @param  line assert_param error line source number.
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line);
#endif /* USE_FULL_ASSERT */

// #endif /* AML_HAL_H_ Define guard if you prefer this style over #pragma once */
// // Note: #pragma once is generally sufficient for modern compilers.