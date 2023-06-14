#include "global.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <gpio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stream.h>
#include <uart.h>
#include <util/delay.h>

#define print_p(s) UART_printStr_p(PSTR(s))
#define print(s)   UART_printStr(s)
#define printErr(s, e)     \
    print_p(s ": ");       \
    print(itoa(e, s, 16)); \
    print_p("\n");

#define SWUART_TX_PIN            GPIO_A4
#define SWUART_RX_PIN            GPIO_A5

#define SWUART_BAUD              115200
#define SWUART_BIT_DELAY         _delay_us(1E6 / SWUART_BAUD - 0.56)
#define SWUART_HALF_BIT_DELAY    _delay_us(1E6 / SWUART_BAUD / 2)
#define SWUART_QUARTER_BIT_DELAY _delay_us(1E6 / SWUART_BAUD / 4)

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

void UARTSW_Init()
{
    GPIO_setValueHigh(&(GPIO_TypeDef)SWUART_TX_PIN);
    GPIO_setValueHigh(&(GPIO_TypeDef)SWUART_RX_PIN);
    GPIO_setOutput(&(GPIO_TypeDef)SWUART_TX_PIN);

    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(PCINT13);
}

bool UARTSW_TransmiteByte(uint8_t b, bool blocking)
{
    // send the start bit
    GPIO_setValueLow(&(GPIO_TypeDef)SWUART_TX_PIN);
    SWUART_BIT_DELAY;
    for (uint8_t i = 1; i; i <<= 1)
    {
        GPIO_setValueLogical(&(GPIO_TypeDef)SWUART_TX_PIN, (i & b));
        SWUART_BIT_DELAY;
    }
    GPIO_setValueHigh(&(GPIO_TypeDef)SWUART_TX_PIN);
    SWUART_BIT_DELAY;
    return true;
}

stream_t uartsw = STREAM_CREATE(UARTSW_TransmiteByte, NULL);

void UARTSW_printStr_p(const char *s) { STREAM_printStr_p(s, &uartsw); }

ISR(PCINT1_vect) {}

int main(void)
{

    UART_init();
    print_p("Program Started\n");

    UARTSW_Init();

    for (;;)
    {
        UARTSW_printStr_p(PSTR("Hello, World\n"));
        _delay_ms(1);
    }

    return 0;
}
