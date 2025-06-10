#pragma once

#include "aml_hal.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>

// Include uart_handler.h so that functions in gpio_handler.c can use WriteUART_Blocking()
#include "uart_handler.h"

#define DEBOUNCE_DELAY_MS 100    // Debounce time in milliseconds (50ms is a good starting value)

typedef struct {
    Pin_t pin;
    u8 state; // State of the pin, either HIGH or LOW
} DigitalOut;

/**
 * @brief Structure to handle a debounced digital input.
 */
typedef struct {
    Pin_t pin;                  ///< The GPIO port and pin for this input.
    volatile u8 state;          ///< The current stable, debounced state of the pin (HIGH or LOW).
    u8 reading;                 ///< The raw, unstable reading from the last physical pin read.
    u32 last_debounce_time;     ///< The timestamp when the pin first started to change state.
    u8 last_stable_state;       ///< The previous debounced state, to detect when a change occurs.
} DigitalIn;

// --- Function Prototypes ---

void MX_GPIO_Init(void);

// Digital Output Functions
DigitalOut AssignDigitalOut(Pin_t pin);
void WriteDigitalOut(DigitalOut *digital_out, u8 state);
void ToggleDigitalOut(DigitalOut *digital_out);
void BuiltinLED_Init(void);
void indicate_setup_with_blinks(int num_blinks, u32 delay_ms);
void ToggleLED(DigitalOut *led, u32 interval, u32 *last_tick_timer);

// Digital Input Functions
DigitalIn AssignDigitalIn(Pin_t pin); // Corrected prototype for polling setup
void AssignDigitalIn_IT(Pin_t pin);    // Corrected prototype for IT setup (returns void)
void DigitalIn_Init(DigitalIn *digital_in, Pin_t pin);
void ReadDigitalIn(DigitalIn *digital_in);
void BuiltinPushButton_Init(void);

// HAL Callback Prototype
void HAL_GPIO_EXTI_Callback(u16 GPIO_Pin);

// --- Extern Variable Declarations ---
extern DigitalOut g_builtinLed;
extern u32 g_ledToggleTimer;
extern DigitalIn g_builtinPushButton;
extern volatile bool g_is_logging_active; // Changed to volatile
