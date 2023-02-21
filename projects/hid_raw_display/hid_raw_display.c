#include "global.h"
#include <usb/usb_rawhid.h>

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
uint8_t buf[64];

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

    for (;;)
    {
        if (usb_rawhid_recv(buf, 0) > 0)
        {
            fprintf_P(&lcd, PSTR("%s\n"), (char *)buf);
        }
    }
    return 0;
}
