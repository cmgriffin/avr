#include "global.h"
#include <debug_minimal.h>
#include <gpio.h>
#include <uart.h>
#include <util/delay.h>

int main(void)
{
    UART_init();
    GPIO_setOutput(&(GPIO_TypeDef)GPIO_D13);
    DEBUG_println("led pin set to output");

    uint16_t ntimes = 0;

    for (;;)
    {
        GPIO_toggleValue(&(GPIO_TypeDef)GPIO_D13);
        ntimes++;

        DEBUG_print("led toggled: ");
        DEBUG_printnum(ntimes);
        DEBUG_print("times\n");
        _delay_ms(1000);
        }

    return 0;
}
