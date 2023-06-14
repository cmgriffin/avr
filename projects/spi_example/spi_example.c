#include "global.h"
#include <gpio.h>
#include <spi.h>
#include <stdio.h>
// #include <uart.h>
#include <util/delay.h>

int main(void)
{
    GPIO_setOutput(&(GPIO_TypeDef)GPIO_D10);
    const SPI_Init_Typedef init = {.interuptEn       = false,
                                   .dataOrderLsb     = false,
                                   .masterSelect     = true,
                                   .clkPolHigh       = false,
                                   .clkPhaseTrailing = false,
                                   .clkDoubleSpeed   = true,
                                   .clkSelect        = SPI_SPCR_CLK_DIV4};
    SPI_init(&init);

    for (;;)
    {
        GPIO_setValueLow(&(GPIO_TypeDef)GPIO_D10);
        SPI_transferWord(0xAA55);
        GPIO_setValueHigh(&(GPIO_TypeDef)GPIO_D10);
    }
    return 0;
}
