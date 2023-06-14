#include "global.h"
#include <avr/pgmspace.h>
#include <avrlibdefs.h>
#include <stdio.h>
#include <uart.h>
#include <util/delay.h>

struct test
{
    uint16_t word;
    uint8_t byte;
    uint8_t str[10];
};

void printBuffer(const void *const buf, uint8_t len)
{
    printf_P(PSTR("[ "));
    for (uint8_t i = 0; i < len; i++)
    {
        uint8_t b = *((uint8_t *)buf + i);
        printf_P(PSTR("%02X, "), b);
    }
    printf_P(PSTR("]\n"));
}

int main(void)
{
    UART_init();

    struct test s = {.word = 0x0011, .byte = 0x55, .str = "012345678"};

    printBuffer(&s, sizeof(struct test));

    for (;;)
    {
    }
    return 0;
}
