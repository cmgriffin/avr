#include "spi.h"

void SPI_init(const SPI_Init_Typedef *init)
{
    // ATMEGA8 series specific pins for the hardware spi peripheral
    GPIO_setOutput(&((GPIO_TypeDef)GPIO_PB3));    // set mosi to output
    GPIO_setOutput(&((GPIO_TypeDef)GPIO_PB5));    // set sck to output
    GPIO_setValueHigh(&((GPIO_TypeDef)GPIO_PB4)); // set miso to input pullup

    SPCR |= SPI_SPCR_SPI_EN | (init->clkSelect) |
            (init->interuptEn ? SPI_SPCR_INTERUPT_EN : 0) |
            (init->dataOrderLsb ? SPI_SPCR_DATA_ORDER_LSB : 0) |
            (init->masterSelect ? SPI_SPCR_MASTER_MODE : 0) |
            (init->clkPolHigh ? SPI_SPCR_CLK_POL : 0) |
            (init->clkPhaseTrailing ? SPI_SPCR_CLK_PH : 0);
    SPSR |= init->clkDoubleSpeed ? SPI_SPSR_CLK_DOUBLE : 0;
}

uint8_t SPI_transferByte(uint8_t byte)
{
    SPDR = byte;
    loop_until_bit_is_set(SPSR, SPIF);
    byte = SPDR;
    return byte;
}
uint16_t SPI_transferWord(uint16_t word)
{
    uint16_t rxData = 0;
    rxData          = (uint16_t)SPI_transferByte((uint8_t)(word >> 8)) << 8;
    rxData |= (uint16_t)SPI_transferByte((uint8_t)(word & 0x00ff));
    return rxData;
}
