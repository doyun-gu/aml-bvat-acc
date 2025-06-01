#pragma once

#include "aml_hal.h"
#include "stm32f4xx_hal.h"
#include "timer.h"
#include "uart_handler.h"
#include "i2c_handler.h"
#include "gpio_handler.h"
#include "file_handler.h"

#include <stdio.h>
#include <string.h>

// void log_to_sd(u32 timestamp, u8 x, u8 y, u8 z);
void measure_loop(void);