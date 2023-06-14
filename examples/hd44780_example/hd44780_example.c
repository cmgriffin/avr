#include "global.h"
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
        c, &((stream_t){.tx_func = HD44780_printChar, .rx_func = NULL}),
        stream);
}

FILE lcd = FDEV_SETUP_STREAM(LCD_putChar, NULL, _FDEV_SETUP_WRITE);

int main(void)
{

    UART_init();
    HD44780_init();

    fprintf_P(&lcd, PSTR("Hello, World!!!\nHD44780 LCD"));
    _delay_ms(2000);
    HD44780_clear();
    uint32_t counter = 0;

    for (;;)
    {
        fprintf_P(&lcd, PSTR("Counter =\n%16lx"), counter);
        _delay_ms(10);
        HD44780_setCursor(0, 0);
        counter++;
    }
    return 0;
}
