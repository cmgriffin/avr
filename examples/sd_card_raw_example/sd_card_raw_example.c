#include "global.h"
#include <avr/pgmspace.h>
#include <sd/sd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <uart.h>
#include <util/delay.h>

#define print_p(s) UART_printStr_p(PSTR(s))
#define print(s)   UART_printStr(s)
#define printErr(s, e)     \
    print_p(s ": ");       \
    print(itoa(e, s, 16)); \
    print_p("\n");

#define RESTORE_DATA                                                          \
    {                                                                         \
        76, 2, 205, 19, 234, 0, 124, 0, 0, 235, 254, 0, 0, 0, 0, 0, 0, 0, 0,  \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 51, 30, 1, 0, 0, 0, 128, 32, 33, 0, 12, 254, 255,  \
            255, 0, 8, 0, 0, 0, 196, 218, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 85, 170              \
    }

char nibbleToHexCharacter(uint8_t nibble)
{
    /* Converts 4 bits into hexadecimal */
    if (nibble < 10)
    {
        return ('0' + nibble);
    }
    else
    {
        return ('A' + nibble - 10);
    }
}

void printHexByte(uint8_t byte)
{
    /* Prints a byte as its hexadecimal equivalent */
    uint8_t nibble;
    nibble = (byte & 0b11110000) >> 4;
    UART_TransmitByte(nibbleToHexCharacter(nibble), true);
    nibble = byte & 0b00001111;
    UART_TransmitByte(nibbleToHexCharacter(nibble), true);
}

void printBuf(uint8_t *buf)
{
    for (uint16_t i = 0; i < 512; i++)
    {
        if (i % 16 == 0)
        {
            print_p("\n");
            printHexByte(i >> 8);
            printHexByte(i & 0xFF);
            print_p(": ");
        }
        printHexByte(buf[i]);
        print_p(" ");
    }
    print_p("\n");
}

int main(void)
{

    UART_init();
    print_p("Program Started\n");

    SD_RETURN_CODES return_code = 0;
    char s[10];
    uint8_t buf[512] = {0}, old_buf[512] = RESTORE_DATA;

    if ((return_code = sd_init()) != SD_OK)
    {
        printErr("sd_init error", return_code);
    }

    print_p("read starting values in first block:\n");

    if ((return_code = sd_read_single_block(0, buf)) != 0xfe)
    {
        printErr("sd_read error", return_code);
    }

    printBuf(buf);

    // for (uint16_t i = 0; i < 512; i++)
    // {
    //     old_buf[i] = buf[i];
    //     buf[i]     = i % 2 ? 0xAA : 0x55;
    // }

    // print_p("buffer values after modifying the block:\n");
    // printBuf(old_buf);

    // if ((return_code = sd_write_single_block(0, old_buf)) != 0x05)
    // {
    //     printErr("sd_write error", return_code);
    // }

    // print_p("read modified values in first block:\n");

    // if ((return_code = sd_read_single_block(0, buf)) != 0xfe)
    // {
    //     printErr("sd_read error", return_code);
    // }

    // printBuf(buf);

    // if ((return_code = sd_write_single_block(0, old_buf)) != 0x05)
    // {
    //     printErr("sd_write error", return_code);
    // }

    // printBuf(buf);

    for (;;)
    {
    }

    return 0;
}
