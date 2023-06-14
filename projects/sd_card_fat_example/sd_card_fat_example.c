#include "global.h"
#include <avr/pgmspace.h>
#include <sd/fat.h>
#include <sd/sd.h>
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
    char s[10];

    // SD_RETURN_CODES sd_code = 0;

    // sd_code                 = sd_init();

    // print_p("sd init failed return code: ");
    // print(itoa(sd_code, s, 10));
    // print_p("\n");

    // return 0;

    // Mount the sd card
    return_code = FAT_mountVolume();

    // If no error
    if (return_code == MR_OK)
    {
        print_p("Volume successfully mounted\n");

        // // Make a directory
        // FAT_makeDir("/Logging");

        // // Open folder
        return_code = FAT_openDir(&dir, "/");
        print_p("dir successfully opened\n");
        // ... optionally print folder name using an LCD library:

        // Get number of folders and files inside the directory
        dirItems = FAT_dirCountItems(&dir);
        print_p("number of items in dir: ");
        print(itoa(dirItems, s, 10));
        print_p("\n");

        // Open a file for reading or writing
        return_code = FAT_fopen(&dir, &file, "/log.txt");

        // Make the file if it doesn't exist
        // The functions that make folders and files takes much space
        // so it is recommended to have them made on a PC instead
        if (return_code == FR_NOT_FOUND)
        {
            print_p("file did not exist, creating it\n");
            FAT_makeFile("/log.txt");
            return_code = FAT_fopen(&dir, &file, "/log.txt");
        }
        else
        {
            print_p("file did exist\n");
        }

        print_p("current filename: \n");
        print(FAT_getFilename());
        print_p("\n");

        print_p("file is a file: \n");
        print(FAT_attrIsFile(&file) ? "True" : "False");
        print_p("\n");

        // Move the writing pointer to the end of the file
        FAT_fseekEnd(&file);
        // Write a string
        return_code = FAT_fwriteString(&file, "Logging Date: 2022\n");
        print_p("rtn of writestring: ");
        print(itoa(return_code, s, 10));
        print_p("\n");

        // Write sensor output
        // FAT_fwriteFloat(&file, 120.033, 3);
        // FAT_fwriteString(&file, ",");
        // FAT_fwriteFloat(&file, -0.221, 3);
        // FAT_fwriteString(&file, ",");
        // FAT_fwriteFloat(&file, -30.004, 3);
        // FAT_fwriteString(&file, ",");
        // FAT_fwriteFloat(&file, 0.023, 3);

        // Synchronize the writing buffer with the card
        return_code = FAT_fsync(&file);
        print_p("rtn of sync: ");
        print(itoa(return_code, s, 10));
        print_p("\n");
    }
    else
    {

        print_p("Volume mount failed return code: ");
        print(itoa(return_code, s, 10));
        print_p("\n");
    }

    for (;;)
    {
    }

    return 0;
}
