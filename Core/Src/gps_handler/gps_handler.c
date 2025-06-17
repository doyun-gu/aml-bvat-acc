/* ============================== */
/* gps_handler.c - FINAL VERSION */
/* ============================== */

#include "gps_handler.h"
#include "uart_handler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define GPS_DMA_BUFFER_SIZE 512
#define NMEA_SENTENCE_BUFFER_SIZE 128

static GPS_Data_t s_gps_data;
static uint8_t s_gps_dma_buffer[GPS_DMA_BUFFER_SIZE];
static size_t old_dma_pos = 0;

extern UART_HandleTypeDef huart1;

// New global flag set in ISR
volatile bool gps_data_ready_flag = false;

static void GPS_Parse_RMC_Sentence(char* rmc_sentence);
static float nmea_to_decimal_degrees(const char* nmea_coord, char direction);

void GPS_Init(void) {
    MX_USART1_UART_Init();
    HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
    
    memset(&s_gps_data, 0, sizeof(GPS_Data_t));
    s_gps_data.is_valid_fix = false;
    if (HAL_UART_Receive_DMA(&huart1, s_gps_dma_buffer, GPS_DMA_BUFFER_SIZE) != HAL_OK) {
        WriteUART_Blocking("ERROR: GPS DMA Reception Start FAILED!\r\n");
        Error_Handler();
    }
}

void GPS_Process_Buffer(void) {
    size_t current_dma_pos = GPS_DMA_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx);
    static char nmea_sentence_buffer[NMEA_SENTENCE_BUFFER_SIZE];
    static uint8_t nmea_pos = 0;
    static bool seen_dollar = false;

    while (old_dma_pos != current_dma_pos) {
        char received_char = s_gps_dma_buffer[old_dma_pos];

        if (received_char == '$') {
            nmea_pos = 0;
            memset(nmea_sentence_buffer, 0, sizeof(nmea_sentence_buffer));
            seen_dollar = true;
        }

        if (seen_dollar && nmea_pos < sizeof(nmea_sentence_buffer) - 1) {
            nmea_sentence_buffer[nmea_pos++] = received_char;
        }

        if (received_char == '\n' && seen_dollar) {
            nmea_sentence_buffer[nmea_pos] = '\0';
            seen_dollar = false;
            if (strstr(nmea_sentence_buffer, "RMC") != NULL) {
                GPS_Parse_RMC_Sentence(nmea_sentence_buffer);
            }
            nmea_pos = 0;
        }

        old_dma_pos = (old_dma_pos + 1) % GPS_DMA_BUFFER_SIZE;
    }
}

bool GPS_Get_Latest_Data(GPS_Data_t *gps_data_out) {
    if (gps_data_out != NULL) {
        *gps_data_out = s_gps_data;
    }
    return s_gps_data.is_valid_fix;
}

void Log_Latest_GPS_Data(void) {
    GPS_Data_t gps_data;
    if (GPS_Get_Latest_Data(&gps_data)) {
        char log_buffer[128];
        snprintf(log_buffer, sizeof(log_buffer),
                 "GPS DATA, Speed: %.2f kmh, Fix: Valid, Lat: %.4f, Lon: %.4f\r\n",
                 gps_data.speed_kmh, gps_data.latitude_dd, gps_data.longitude_dd);
        WriteUART_Blocking(log_buffer);
    } 
    
    else {
        WriteUART_Blocking("GPS DATA, Fix: Invalid\r\n");
    }
}

static void GPS_Parse_RMC_Sentence(char* rmc_sentence) {
    char* tokens[15];
    int token_count = 0;
    char sentence_copy[NMEA_SENTENCE_BUFFER_SIZE];

    strncpy(sentence_copy, rmc_sentence, sizeof(sentence_copy) - 1);
    sentence_copy[sizeof(sentence_copy) - 1] = '\0';

    char* token = strtok(sentence_copy, ",");
    while (token != NULL && token_count < 15) {
        tokens[token_count++] = token;
        token = strtok(NULL, ",");
    }

    if (token_count < 9) {
        s_gps_data.is_valid_fix = false;
        return;
    }

    if (strlen(tokens[2]) > 0 && *tokens[2] == 'A') {
        s_gps_data.is_valid_fix = true;

        if(strlen(tokens[1]) > 5) sscanf(tokens[1], "%2hhu%2hhu%2hhu", &s_gps_data.hour, &s_gps_data.minute, &s_gps_data.second);
        if(token_count > 4 && strlen(tokens[3]) > 0) s_gps_data.latitude_dd = nmea_to_decimal_degrees(tokens[3], *tokens[4]);
        if(token_count > 6 && strlen(tokens[5]) > 0) s_gps_data.longitude_dd = nmea_to_decimal_degrees(tokens[5], *tokens[6]);
        if(token_count > 7 && strlen(tokens[7]) > 0) s_gps_data.speed_knots = atof(tokens[7]);
        s_gps_data.speed_kmh = s_gps_data.speed_knots * 1.852f;
        if(token_count > 8 && strlen(tokens[8]) > 0) s_gps_data.course_deg = atof(tokens[8]);

    } else {
        s_gps_data.is_valid_fix = false;
        s_gps_data.speed_knots = 0.0f;
        s_gps_data.speed_kmh = 0.0f;
    }
}

static float nmea_to_decimal_degrees(const char* nmea_coord, char direction) {
    if (nmea_coord == NULL || *nmea_coord == '\0') return 0.0f;

    float raw_value = atof(nmea_coord);
    int degrees = (int)(raw_value / 100.0f);
    float minutes = raw_value - (degrees * 100.0f);
    float decimal_degrees = degrees + (minutes / 60.0f);

    if (direction == 'S' || direction == 'W') decimal_degrees *= -1.0f;

    return decimal_degrees;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        gps_data_ready_flag = true;
    }
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        gps_data_ready_flag = true;
    }
}