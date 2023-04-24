#include "global.h"
#include <gpio.h>
#include <util/delay.h>

int main(void)
{
    GPIO_setOutput(&(GPIO_TypeDef)GPIO_D13);

    for (;;)
    {
        GPIO_toggleValue(&(GPIO_TypeDef)GPIO_D13);
        _delay_ms(1000);
    }

    return 0;
}
