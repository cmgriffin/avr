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
        c,
        &((stream_t){.tx_func = HD44780_printCharScrolling, .rx_func = NULL}),
        stream);
}

FILE lcd = FDEV_SETUP_STREAM(LCD_putChar, NULL, _FDEV_SETUP_WRITE);

int main(void)
{

    UART_init();
    HD44780_init();
    HD44780_setCursor(0, 1);
    fprintf_P(&lcd, PSTR("Hello, World!!!\nHD44780 LCD\n"));
    _delay_ms(1000);
    fprintf_P(&lcd, PSTR("these are lines\n"));
    _delay_ms(1000);
    fprintf_P(&lcd, PSTR("written to the\n"));
    _delay_ms(1000);
    fprintf_P(&lcd, PSTR("LCD\n"));
    _delay_ms(1000);
    HD44780_clear();
    uint32_t counter = 0;

    for (;;)
    {
        fprintf_P(&lcd, PSTR("Counter = %lx\n"), counter);
        _delay_ms(500);
        // HD44780_setCursor(0, 0);
        counter++;
    }
    return 0;
}
