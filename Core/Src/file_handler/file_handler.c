// #include "file_handler.h"

// /**
//  * @brief Create a csv file with the given name and write the header to it.
//  */
// void create_csv_file(const char *filename) {
//     FIL file;
//     FRESULT res;

//     // Open the file for writing
//     res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
//     if (res != FR_OK) {
//         WriteUART("Failed to create file.\n");
//         return;
//     }

//     // Write the header to the file
//     const char *header = "Time, x, y, z\n";

//     UINT bytes_written;

//     res = f_write(&file, header, strlen(header), &bytes_written);

//     if (res != FR_OK || bytes_written < strlen(header)) {
//         WriteUART("Failed to write header to file.\n");
//         f_close(&file);
//         return;
//     }

//     // Close the file
//     f_close(&file);
//     WriteUART("CSV file created successfully.\n");
// }

