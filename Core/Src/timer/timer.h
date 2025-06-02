#pragma once

/**
 * Libraries -------------------------------------------------
 */
#include <stdbool.h>

#include "aml_hal.h"
#include "i2c_handler.h"
#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim2;
// extern I2C_HandleTypeDef hi2c1;


void SystemClock_Config(void);
void MX_TIM2_Init(void);
void TIM2_IRQHandler(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
bool timer_handler(u32 interval, u32 *timer);