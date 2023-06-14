#include "global.h"
#include <debug_minimal.h>
#include <gpio.h>
#include <uart.h>
#include <util/delay.h>

int main(void)
{
    UART_init();
    GPIO_setOutput(&(GPIO_TypeDef)GPIO_D13);
    DEBUG_printstr("led pin set to output\n");

    uint16_t ntimes = 0;

    for (;;)
    {
        GPIO_toggleValue(&(GPIO_TypeDef)GPIO_D13);
        ntimes++;

        DEBUG_printstr("led toggled: ");
        DEBUG_printnum(ntimes);
        DEBUG_printstr("times\n");
        _delay_ms(1000);
    }

    return 0;
}
