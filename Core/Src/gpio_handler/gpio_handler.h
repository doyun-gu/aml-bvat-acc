#pragma once

#include "aml_hal.h"
#include "stm32f4xx_hal.h"

typedef struct {
    Pin_t pin;
    u8 state; // State of the pin, either HIGH or LOW
} DigitalOut;

// Function prototypes for UART initialisation and handling
void MX_GPIO_Init(void);
DigitalOut AssignDigitalOut(Pin_t pin);
void WriteDigitalOut(DigitalOut *digital_out, u8 state);
void ToggleDigitalOut(DigitalOut *digital_out);
void BuiltinLED_Init(void);
void indicate_setup_with_blinks(int num_blinks, u32 dealy_ms);
void ToggleLED(DigitalOut *led, u32 interval, u32 *last_tick_timer);

// Extern declarations for global variables
extern DigitalOut g_builtinLed;
extern u32 g_ledToggleTimer;