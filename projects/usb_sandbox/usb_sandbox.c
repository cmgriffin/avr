#include "global.h"
#include <usb/usb_hid_debug.h>

#include <avr/pgmspace.h>
#include <devices/hd44780.h>
#include <stdio.h>
#include <stdlib.h>
#include <stream.h>
#include <uart.h>
#include <util/delay.h>

int LCD_putChar(char c, FILE *stream)
{
    return STREAM_putChar(
        c,
        &((stream_t){.tx_func = HD44780_printCharScrolling, .rx_func = NULL}),
        stream);
}

int USB_putChar(char c, FILE *stream) { return (int)usb_debug_putchar(c); }

FILE lcd = FDEV_SETUP_STREAM(LCD_putChar, NULL, _FDEV_SETUP_WRITE);
FILE usb = FDEV_SETUP_STREAM(USB_putChar, NULL, _FDEV_SETUP_WRITE);

int main(void)
{
    usb_init();
    while (!usb_configured())
    {
    }

    // UART_init();
    HD44780_init();
    // stdout = &lcd;
    fprintf_P(&lcd, PSTR("Hello, World!!!\nHD44780 LCD\n"));
    _delay_ms(2000);
    uint32_t counter = 0;

    for (;;)
    {
        fprintf_P(&usb, PSTR("Counter = %lx\n"), counter);
        fprintf_P(&lcd, PSTR("Counter = %lx\n"), counter);
        _delay_ms(500);
        // HD44780_setCursor(0, 0);
        counter++;
    }
    return 0;
}
