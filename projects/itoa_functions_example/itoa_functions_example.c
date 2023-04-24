#include "global.h"
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <uart.h>
#include <util/delay.h>

#include "conv2str.h"

#define print_p(s) UART_printStr_p(PSTR(s))
#define print(s)   UART_printStr(s)

int main(void)
{
    UART_init();
    print_p("Program Started\n");

    char s[10];
    for (int16_t v = 1; !(v & 0x4000); v *= 2)
    {
        print(itoa(v, s, 10));
        print_p(": ");

        for (uint8_t fp = 0; fp < 8; fp++)
        {
            print(CONV2STR_fptostr31(s, v));
            print_p(" ");
        }
        print_p("\n");
    }

    for (;;)
    {
    }

    return 0;
}
