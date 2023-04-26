#include "global.h"
#include <avr/pgmspace.h>
#include <sd/fat.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <uart.h>
#include <util/delay.h>

#define print_p(s) UART_printStr_p(PSTR(s))
#define print(s)   UART_printStr(s)

int main(void)
{
    UART_init();
    print_p("Program Started\n");

    FAT_DIR dir;
    FAT_FILE file;
    uint8_t return_code = 0;
    uint8_t dirItems    = 0;

    // Mount the sd card
    return_code = FAT_mountVolume();

    // If no error
    if (return_code == MR_OK)
    {
        // Make a directory
        FAT_makeDir("/Logging");

        // Open folder
        return_code = FAT_openDir(&dir, "/Logging");
        if (return_code == FR_OK)
        {
            // ... optionally print folder name using an LCD library:
            // print(FAT_getFilename());

            // Get number of folders and files inside the directory
            dirItems = FAT_dirCountItems(&dir);

            // Open a file for reading or writing
            return_code = FAT_fopen(&dir, &file, "log.txt");

            // Make the file if it doesn't exist
            // The functions that make folders and files takes much space
            // so it is recommended to have them made on a PC instead
            if (return_code == FR_NOT_FOUND)
            {
                FAT_makeFile("/Logging/log.txt");
                return_code = FAT_fopen(&dir, &file, "log.txt");
            }

            // Move the writing pointer to the end of the file
            FAT_fseekEnd(&file);

            // Write a string
            FAT_fwriteString(&file, "Logging Date: 2022\n");

            // Write sensor output
            FAT_fwriteFloat(&file, 120.033, 3);
            FAT_fwriteString(&file, ",");
            FAT_fwriteFloat(&file, -0.221, 3);
            FAT_fwriteString(&file, ",");
            FAT_fwriteFloat(&file, -30.004, 3);
            FAT_fwriteString(&file, ",");
            FAT_fwriteFloat(&file, 0.023, 3);

            // Synchronize the writing buffer with the card
            FAT_fsync(&file);
        }
    }

    for (;;)
    {
    }

    return 0;
}
