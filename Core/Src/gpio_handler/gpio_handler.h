#pragma once

#include "aml_hal.h"
#include "stm32f4xx_hal.h"

typedef struct {
    Pin pin;
    u8 state; // State of the pin, either HIGH or LOW
} DigitalOut;

// Function prototypes for UART initialization and handling
void MX_GPIO_Init(void);
void AssignDigitalOut(Pin pin);
void WriteDigitalOut(DigitalOut *digital_out, u8 state);
void ToggleDigitalOut(DigitalOut *digital_out);