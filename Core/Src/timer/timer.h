#pragma once

/**
 * Libraries -------------------------------------------------
 */
#include <stdbool.h>

#include "aml_hal.h"
#include "stm32f4xx_hal.h"



void SystemClock_Config(void);
// void MX_TIM2_Init(void);

bool timer_handler(u32 interval, u32 *timer);