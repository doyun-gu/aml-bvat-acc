#pragma once 

#include "stm32f4xx_hal.h"

/** ----------------------------------------Data Declaration--------------------------------------------  
 * 
 * @brief Declare all the necessary data types for the AML library
 * ! Should add this later when all the functions all declared completely !
*/
typedef uint8_t  u8;     /**< unsigned 8-bit integer */
typedef uint16_t u16;    /**< unsigned 16-bit integer */
typedef uint32_t u32;    /**< unsigned 32-bit integer */
typedef int8_t   s8;     /**< signed 8-bit integer */
typedef int16_t  s16;    /**< signed 16-bit integer */
typedef int32_t  s32;    /**< signed 32-bit integer */
typedef uint8_t  bool_t; /**< boolean type, 0 or 1 */

/**  -----------------------------------------Pin Configuration----------------------------------------- 
 * @brief Organise all the pins on the STM32F board
 * @note This section will declare the pin configuration for STM32F401RE and STM32F411RE nucleo Board
 */

// Struct for holding a pin's port and number
typedef struct {
    GPIO_TypeDef *port;
    u16 pin;
} Pin;

#define D0_PORT GPIOA
#define D0_PIN GPIO_PIN_3

#define D1_PORT GPIOA
#define D1_PIN GPIO_PIN_2

#define D2_PORT GPIOA
#define D2_PIN GPIO_PIN_10

#define D3_PORT GPIOB
#define D3_PIN GPIO_PIN_3

#define D4_PORT GPIOB
#define D4_PIN GPIO_PIN_5

#define D5_PORT GPIOB
#define D5_PIN GPIO_PIN_4

#define D6_PORT GPIOB
#define D6_PIN GPIO_PIN_10

#define D7_PORT GPIOB
#define D7_PIN GPIO_PIN_8

#define D8_PORT GPIOA
#define D8_PIN GPIO_PIN_9

#define D9_PORT GPIOC
#define D9_PIN GPIO_PIN_7

#define D10_PORT GPIOB
#define D10_PIN GPIO_PIN_6

#define D11_PORT GPIOA
#define D11_PIN GPIO_PIN_7

#define D12_PORT GPIOA
#define D12_PIN GPIO_PIN_6

#define D13_PORT GPIOA
#define D13_PIN GPIO_PIN_5

#define D14_PORT GPIOB
#define D14_PIN GPIO_PIN_9

#define D15_PORT GPIOB
#define D15_PIN GPIO_PIN_8

#define A0_PORT GPIOA
#define A0_PIN GPIO_PIN_0

#define A1_PORT GPIOA
#define A1_PIN GPIO_PIN_1

#define A2_PORT GPIOA
#define A2_PIN GPIO_PIN_4

#define A3_PORT GPIOB
#define A3_PIN GPIO_PIN_0

#define A4_PORT GPIOC
#define A4_PIN GPIO_PIN_1

#define A5_PORT GPIOC
#define A5_PIN GPIO_PIN_0

#define D0 \
    (Pin) {D0_PORT, D0_PIN}

#define D1 \
    (Pin) {D1_PORT, D1_PIN}

#define D2 \
    (Pin) {D2_PORT, D2_PIN}

#define D3 \
    (Pin) {D3_PORT, D3_PIN}

#define D4 \
    (Pin) {D4_PORT, D4_PIN}

#define D5 \
    (Pin) {D5_PORT, D5_PIN}

#define D6 \
    (Pin) {D6_PORT, D6_PIN}

#define D7 \
    (Pin) {D7_PORT, D7_PIN}

#define D8 \
    (Pin) {D8_PORT, D8_PIN}

#define D9 \
    (Pin) {D9_PORT, D9_PIN}

#define D10 \
    (Pin) {D10_PORT, D10_PIN}

#define D11 \
    (Pin) {D11_PORT, D11_PIN}

#define D12 \
    (Pin) {D12_PORT, D12_PIN}

#define D13 \
    (Pin) {D13_PORT, D13_PIN}

#define D14 \
    (Pin) {D14_PORT, D14_PIN}

#define D15 \
    (Pin) {D15_PORT, D15_PIN}

#define A0 \
    (Pin) {A0_PORT, A0_PIN}

#define A1 \
    (Pin) {A1_PORT, A1_PIN}

#define A2 \
    (Pin) {A2_PORT, A2_PIN}

#define A3 \
    (Pin) {A3_PORT, A3_PIN}

#define A4 \
    (Pin) {A4_PORT, A4_PIN}

#define A5 \
    (Pin) {A5_PORT, A5_PIN}

/** 
 * ----------------------------------------Function Declaration-----------------------------------------
 * @brief Functions to initialise
 */
void AML_Init(void);
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

void MX_I2C1_Init(void);
