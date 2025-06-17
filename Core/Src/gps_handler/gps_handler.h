#pragma once

//==============================================================================
// Includes
//==============================================================================
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "aml_hal.h"

//==============================================================================
// Data Structures and Enums
//==============================================================================
/**
 * @brief Structure to hold processed GPS data.
 */
typedef struct {
    float latitude_dd;
    float longitude_dd;
    float speed_knots;
    float speed_kmh;
    float course_deg;
    bool is_valid_fix;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} GPS_Data_t;

//==============================================================================
// Function Prototypes
//==============================================================================
void GPS_Init(void);
void GPS_Process_Buffer(void); // This is now called by interrupt callbacks
bool GPS_Get_Latest_Data(GPS_Data_t *gps_data_out);
void Log_Latest_GPS_Data(void); // <<<--- NEW PROTOTYPE ADDED HERE

// HAL UART Rx Callbacks (Implemented in gps_handler.c)
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart);
