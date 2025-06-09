#pragma once

#include "aml_hal.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define DEBOUNCE_DELAY_MS 50    // Debounce time in milliseconds (50ms is a good starting value)

typedef struct {
    Pin_t pin;
    u8 state; // State of the pin, either HIGH or LOW
} DigitalOut;

/**
 * @brief Structure to handle a debounced digital input.
 * Can be used in a polling loop.
 */
typedef struct {
    // --- Core Pin Configuration ---
    Pin_t pin;                  ///< The GPIO port and pin for this input.

    // --- Debounced State ---
    volatile u8 state;          ///< The current stable, debounced state of the pin (HIGH or LOW).
                                ///< 'volatile' is good practice if read in main loop and changed in an ISR.

    // --- Internal State Machine for Debouncing/Edge Detection ---
    u8 reading;                 ///< The raw, unstable reading from the last physical pin read.
    u32 last_debounce_time;     ///< The timestamp (from HAL_GetTick()) when the pin first changed state.
    
    // --- Optional for Edge Detection (detecting a press/release event) ---
    u8 last_stable_state;       ///< The previous debounced state, to detect when a change occurs.
    
} DigitalIn; // Using _t suffix for type definitions is a common convention

// Function prototypes for UART initialisation and handling
void MX_GPIO_Init(void);
DigitalOut AssignDigitalOut(Pin_t pin);
void WriteDigitalOut(DigitalOut *digital_out, u8 state);
void ToggleDigitalOut(DigitalOut *digital_out);
void BuiltinLED_Init(void);
void indicate_setup_with_blinks(int num_blinks, u32 dealy_ms);
void ToggleLED(DigitalOut *led, u32 interval, u32 *last_tick_timer);

void AssignDigitalIn(DigitalIn *digital_in, Pin_t pin);
void ReadDigitalIn(DigitalIn *digital_in);

// Extern declarations for global variables

// Extern declaration for digital output
extern DigitalOut g_builtinLed;
extern u32 g_ledToggleTimer;

// Extern declaration for digital reading
extern DigitalIn g_builtinPushButton;     // Global variable for the push button, defined in gpio_handler.c
extern bool g_is_logging_active; // Global flag to indicate if logging is active