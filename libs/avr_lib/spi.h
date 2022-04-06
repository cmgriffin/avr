#ifndef __SPI__
#define __SPI__

#include "avrlibdefs.h"
#include "gpio.h"

#ifdef SPCR

#define SPI_SPCR_INTERUPT_EN    _BV(SPIE)
#define SPI_SPCR_SPI_EN         _BV(SPE)
#define SPI_SPCR_DATA_ORDER_LSB _BV(DORD)
#define SPI_SPCR_MASTER_MODE    _BV(MSTR)
#define SPI_SPCR_CLK_POL        _BV(CPOL)
#define SPI_SPCR_CLK_PH         _BV(CPHA)
#define SPI_SPCR_CLK_DIV4       0
#define SPI_SPCR_CLK_DIV16      1
#define SPI_SPCR_CLK_DIV64      2
#define SPI_SPCR_CLK_DIV128     3

#define SPI_SPSR_INTERUPT_FLAG  _BV(SPIF)
#define SPI_SPSR_COLLISION_FLAG _BV(WCOL)
#define SPI_SPSR_CLK_DOUBLE     _BV(SPI2X)

typedef struct
{
    bool interuptEn       : 1;
    bool dataOrderLsb     : 1;
    bool masterSelect     : 1;
    bool clkPolHigh       : 1;
    bool clkPhaseTrailing : 1;
    bool clkDoubleSpeed   : 1;
    uint8_t clkSelect;

} SPI_Init_Typedef;

/**
 * @brief initialize the spi peripheral
 *
 * @param init Initialization configuration parameters
 */
void SPI_init(const SPI_Init_Typedef *init);

/**
 * @brief send/receive a byte with the spi peripheral
 *
 * CS/SS must be controlled byte user application
 *
 * @param byte byte to send
 * @return uint8_t received byte
 */
uint8_t SPI_transferByte(uint8_t byte);

/**
 * @brief send/recieve a word (2B) with the spi peripheral
 *
 * CS/SS must be controlled byte user application
 *
 * @param word word to send
 * @return uint16_t recieved word
 */
uint16_t SPI_transferWord(uint16_t word);

#endif // SPCR

#endif // __SPI__