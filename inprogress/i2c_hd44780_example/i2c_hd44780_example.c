#include "global.h"
#include <avr/pgmspace.h>
// #include <devices/pfc8574.h>
#include <devices/hd44780.h>
#include <i2c.h>
#include <stdbool.h>
#include <stdint.h>
// #include <stdio.h>
#include <stdlib.h>
#include <stream.h>
#include <uart.h>
#include <util/delay.h>

#define DEBUG
#include <debug_minimal.h>

#define display_print(s) STREAM_printStr_p(PSTR(s), &disp)

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

stream_t disp = {.rx_func = NULL, .tx_func = HD44780_printChar};

const HD44780_CustChar_t degree PROGMEM = HD44780_SYM_DEGREE;
const HD44780_CustChar_t therm PROGMEM  = HD44780_SYM_THERMOMETER;

int main(void)
{
    UART_init();
    _delay_ms(1000);
    DEBUG_println("Program Started\n");

    I2C_init(&(I2C_Init_Typedef){
        .clkSelect = 0, .bitRateSelect = 32, .interuptEn = false});
    I2C_setBitRate(100);

    HD44780_init();

    DEBUG_println("finished init\n");

    HD44780_registerChar(1, &therm);
    HD44780_registerChar(2, &degree);
    display_print("\1Hello0\2");
    HD44780_setCursor(0, 1);
    display_print("\1Hello1\2");
    HD44780_setCursor(0, 2);
    display_print("\1Hello2\2");
    HD44780_setCursor(0, 3);
    display_print("\1Hello3\2");

    for (;;)
    {

        // test_signals();
    }

    return 0;
}
