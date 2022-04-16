#include "global.h"
#include <devices/hd44780.h>
#include <stdio.h>
#include <stdlib.h>
#include <uart.h>
#include <util/delay.h>

int main(void)
{

    UART_init();
    HD44780_init();

    for (;;)
    {
        char *str = "Hello World!blahblahblah";
        char *c   = str;
        while (*c)
        {
            HD44780_printChar(*c++, true);
        }
        _delay_ms(1000);
        HD44780_setCursor(0, 0);
        str = "this is cool!";
        c   = str;
        while (*c)
        {
            HD44780_printChar(*c++, true);
        }
        _delay_ms(1000);
        HD44780_setCursor(0, 0);
    }
    return 0;
}
