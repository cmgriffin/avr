#include "global.h"
#include <usb/usb_usbtmc.h>

#include <avr/pgmspace.h>
#include <devices/hd44780.h>
#include <stdio.h>
#include <stdlib.h>
#include <stream.h>
#include <uart.h>
#include <util/delay.h>

#define println(msg) printf_P(PSTR(msg "\n"))

int LCD_putChar(char c, FILE *stream)
{
    return STREAM_putChar(
        c,
        &((stream_t){.tx_func = HD44780_printCharScrolling, .rx_func = NULL}),
        stream);
}

FILE lcd = FDEV_SETUP_STREAM(LCD_putChar, NULL, _FDEV_SETUP_WRITE);

int main(void)
{
    HD44780_init();
    stdout = &lcd;
    println("Started program");
    usb_init();
    while (!usb_configured())
    {
    }
    println("Enumerated");
    for (;;)
    {
    }
    return 0;
}
