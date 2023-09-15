#include "global.h"
#if __has_include("i2c_conf.h")
#include "i2c_conf.h"

#include "i2c.h"
#include <debug.h>
#include <gpio.h>

#ifdef I2C_DEBUG
#define _I2C_DEBUG(fmt, args...) \
    {                            \
        _DEBUG(fmt, args);       \
    }
#else
#define _I2C_DEBUG(fmt, args...) \
    {                            \
    }
#endif

// Mask TWSR and return the status value
static inline uint8_t __I2C_getStatus() { return (TWSR & I2C_STATUS_MASK); }

// Wait for the current operation to complete
// TODO: add the option for a timeout
static inline void __I2C_waitForComplete()
{
    loop_until_bit_is_set(TWCR, TWINT);
}

// send an I2C start
static inline void __I2C_start()
{
    TWCR = (_BV(TWINT) | _BV(TWEN) | _BV(TWSTA));
    __I2C_waitForComplete();
}

// send an I2C stop
static inline void __I2C_stop()
{
    TWCR = (_BV(TWINT) | _BV(TWEN) | _BV(TWSTO));
}

// Read a byte and send ACK
static inline uint8_t __I2C_readAck(void)
{
    TWCR = (_BV(TWINT) | _BV(TWEN) | _BV(TWEA));
    __I2C_waitForComplete();
    return (TWDR);
}

// Read a byte and send NACK
static inline uint8_t __I2C_readNoAck(void)
{
    TWCR = (_BV(TWINT) | _BV(TWEN));
    __I2C_waitForComplete();
    return (TWDR);
}

// Write a byte
static inline void __I2C_write(uint8_t data)
{
    TWDR = data;
    TWCR = (_BV(TWINT) | _BV(TWEN)); /* init and enable */
    __I2C_waitForComplete();
}

void I2C_init(I2C_Init_Typedef *init)
{
    TWBR = init->bitRateSelect;
    TWSR |= init->clkSelect;
    TWCR |= I2C_TWCR_TWI_EN | (init->interuptEn ? I2C_TWCR_INTERUPT_EN : 0);
#ifdef I2C_GPIO_SDA
    GPIO_setValueHigh(&((GPIO_TypeDef)I2C_GPIO_SDA)); // set SDA pullup
    GPIO_setValueHigh(&((GPIO_TypeDef)I2C_GPIO_SCL)); // set SDA pullup
#endif
    _I2C_DEBUG("completed init routine", "");
}

void I2C_setBitRate(uint16_t bitRateKhz)
{
    bitRateKhz = bitRateKhz > 400 ? 400 : (bitRateKhz < 10 ? 10 : bitRateKhz);
    uint8_t twbr, clkSel = 0;
    for (uint8_t div = 64, clkSel = 3; div > 0; div >>= 2, clkSel--)
    {
        twbr = ((F_CPU / 1000) - 16 * bitRateKhz + (bitRateKhz * div)) /
               (bitRateKhz * 2 * div);
        if (twbr > (I2C_TWBR_MAX / 4))
        {
            break;
        }
    }
    TWBR = twbr;
    update_bits(TWSR, clkSel, 0x3);
    _I2C_DEBUG("set twbr=%d, clkSel=%d for bitRate=%d", twbr, clkSel,
               bitRateKhz);
}

uint8_t I2C_writeBytes(const uint8_t targetAddr, const uint8_t *writeData,
                       const uint8_t nWriteBytes)
{
    uint8_t status;
    __I2C_start();

    if ((status = __I2C_getStatus()) != I2C_STATUS_MT_START)
    {
        __I2C_stop();
        return status;
    }

    __I2C_write((targetAddr << 1) + 0);

    if ((status = __I2C_getStatus()) != I2C_STATUS_MT_SLA_ACK)
    {
        __I2C_stop();
        return status;
    }

    for (uint8_t i = 0; i < nWriteBytes; i++)
    {
        __I2C_write(writeData[i]);
        if ((status = __I2C_getStatus()) != I2C_STATUS_MT_DATA_ACK)
        {
            __I2C_stop();
            return status;
        }
    }

    __I2C_stop();
    return I2C_STATUS_OK;
}

uint8_t I2C_readBytes(const uint8_t targetAddr, uint8_t *readData,
                      const uint8_t nReadBytes)
{
    uint8_t status;

    __I2C_start();

    if ((status = __I2C_getStatus()) != I2C_STATUS_MT_START)
    {
        __I2C_stop();
        return status;
    }

    __I2C_write((targetAddr << 1) + 1);

    if ((status = __I2C_getStatus()) != I2C_STATUS_MR_SLA_ACK)
    {
        __I2C_stop();
        return status;
    }

    uint8_t i;
    for (i = 0; i < (nReadBytes - 1); i++)
    {
        readData[i] = __I2C_readAck();
    }
    readData[i] = __I2C_readNoAck();
    __I2C_stop();
    return I2C_STATUS_OK;
}

uint8_t I2C_writeReadBytes(const uint8_t targetAddr, const uint8_t *writeData,
                           const uint8_t nWriteBytes, uint8_t *readData,
                           const uint8_t nReadBytes)
{
    uint8_t status;

    __I2C_start();
    if ((status = __I2C_getStatus()) != I2C_STATUS_MT_START)
    {
        __I2C_stop();
        _I2C_DEBUG("failed after start, status=0x%02x", status);
        return status;
    }

    __I2C_write((targetAddr << 1) + 0);
    if ((status = __I2C_getStatus()) != I2C_STATUS_MT_SLA_ACK)
    {
        __I2C_stop();
        _I2C_DEBUG("failed after SLA W, status=0x%02x", status);
        return status;
    }

    for (uint8_t i = 0; i < nWriteBytes; i++)
    {
        __I2C_write(writeData[i]);
        if ((status = __I2C_getStatus()) != I2C_STATUS_MT_DATA_ACK)
        {
            __I2C_stop();
            _I2C_DEBUG("failed after DATA W, status=0x%02x", status);
            return status;
        }
    }

    __I2C_start();
    if ((status = __I2C_getStatus()) != I2C_STATUS_MT_REPEAT_START)
    {
        __I2C_stop();
        _I2C_DEBUG("failed after START 2, status=0x%02x", status);
        return status;
    }

    __I2C_write((targetAddr << 1) + 1);
    if ((status = __I2C_getStatus()) != I2C_STATUS_MR_SLA_ACK)
    {
        __I2C_stop();
        _I2C_DEBUG("failed after SLA R, status=0x%02x", status);
        return status;
    }
    uint8_t i;
    for (i = 0; i < (nReadBytes - 1); i++)
    {
        readData[i] = __I2C_readAck();
    }
    readData[i] = __I2C_readNoAck();
    __I2C_stop();
    return I2C_STATUS_OK;
}

uint8_t I2C_scanForDevices(uint8_t *addrList, const uint8_t maxLength)
{
    uint8_t status;
    uint8_t *listPtr = addrList;
    for (uint8_t addr = 1; addr < 128; addr++)
    {
        __I2C_start();
        // i2cSend((addr<<1)+1);
        __I2C_write((addr << 1) + 0);
        // uint8_t code = i2cCheckStatus(TW_MR_SLA_ACK);
        status = __I2C_getStatus();
        _I2C_DEBUG("addr=0x%02x, status=0x%02x", addr, status);
        __I2C_stop();
        if (status == I2C_STATUS_MT_SLA_ACK)
        {
            *listPtr++ = addr;
            if (listPtr - addrList == maxLength)
            {
                break;
            }
        }
    }
    return (uint8_t)((listPtr - addrList) / sizeof(*addrList));
}

#endif