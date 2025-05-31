#include "command_handler.h"

static u32 command_timer = 0;
char UART_buffer[64];

// TODO 1.: Timestamp logging -> Write to a file not just UART
// TODo 2.: Another Loop that will handle file management

void log_to_sd(u32 timestamp, u8 x, u8 y, u8 z) {
    FIL file;
    UINT bytes_written;
    char buffer[64];

    if (f_open(&file, "data.csv", FA_OPEN_APPEND | FA_WRITE) != FR_OK) {
        WriteUART("SD file open failed.\n");
        return;
    }

    snprintf(buffer, sizeof(buffer), "%lu, %d, %d, %d\n", timestamp, x, y, z);
    f_write(&file, buffer, strlen(buffer), &bytes_written);

    f_close(&file);
}

void measure_loop(void) {
    if (timer_handler(1000, &command_timer)) {
        u8 x = readACC(0x28);
        u8 y = readACC(0x29);
        u8 z = readACC(0x2A);

        u32 timestamp = HAL_GetTick();

        // UART Output
        snprintf(UART_buffer, sizeof(UART_buffer), "X: %d, Y: %d, Z: %d\n", x, y, z);
        WriteUART(UART_buffer);

        // SD Logging
        log_to_sd(timestamp, x, y, z);
    }
}