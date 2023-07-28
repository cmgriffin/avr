#include "global.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <gpio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stream.h>
#include <uart.h>
#include <uartsw.h>
#include <uartswtx.h>
#include <util/delay.h>

#define print_p(s) UART_printStr_p(PSTR(s))
#define print(s)   UART_printStr(s)
#define printErr(s, e)     \
    print_p(s ": ");       \
    print(itoa(e, s, 16)); \
    print_p("\n");

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

// stream_t uartsw = STREAM_CREATE(UARTSW_TransmitByte, UARTSW_ReceiveByte);

// void UARTSW_printStr_p(const char *s) { STREAM_printStr_p(s, &uartsw); }

int main(void)
{

    UART_init();
    print_p("Program Started\n");

    // UARTSWTX_init();
    UARTSW_init();
    uint8_t c;

    for (;;)
    {
        // if (UARTSW_ReceiveByte(&c))
        // {
        //     UART_TransmitByte(c, true);
        //     UARTSW_TransmitByte(c);
        // }
        // UARTSWTX_TransmitByte('A', true);
        UARTSW_TransmitByte('A', true);
        _delay_ms(100);
    }

    return 0;
}
