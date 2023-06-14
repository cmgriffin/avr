#ifndef I2C_H
#define I2C_H

#include "avrlibdefs.h"

#ifdef TWCR

#define I2C_TWCR_INTERUPT_EN          0x1
#define I2C_TWCR_TWI_EN               0x4
#define I2C_TWCR_WRITE_COLLISION_FLAG 0x8
#define I2C_TWCR_STOP_COND            0x10
#define I2C_TWCR_START_COND           0x20
#define I2C_TWCR_ACK_EN               0x40
#define I2C_TWCR_INTERUPT_FLAG        0x80

#define I2C_TWSR_CLK_DIV1             0x0
#define I2C_TWSR_CLK_DIV4             0x1
#define I2C_TWSR_CLK_DIV16            0x2
#define I2C_TWSR_CLK_DIV64            0x3

#define I2C_STATUS_MASK               0xF8
#define I2C_STATUS_MT_START           0x08
#define I2C_STATUS_MT_REPEAT_START    0x10
#define I2C_STATUS_MT_SLA_ACK         0x18
#define I2C_STATUS_MT_SLA_NACK        0x20
#define I2C_STATUS_MT_DATA_ACK        0x28
#define I2C_STATUS_MT_DATA_NACK       0x30
#define I2C_STATUS_MT_ARB_LOST        0x38

#define I2C_STATUS_MR_START           0x08
#define I2C_STATUS_MR_REPEAT_START    0x10
#define I2C_STATUS_MR_ARB_LOST        0x38
#define I2C_STATUS_MR_SLA_ACK         0x40
#define I2C_STATUS_MR_SLA_NACK        0x48
#define I2C_STATUS_MR_DATA_ACK        0x50
#define I2C_STATUS_MR_DATA_NACK       0x58

#define I2C_STATUS_OK                 0x01 // not used by peripheral statuses

#define I2C_TWBR_MAX                  0xFF

typedef struct {
    bool interuptEn : 1;
    uint8_t clkSelect;
    uint8_t bitRateSelect;
} I2C_Init_Typedef;

/**
 * @brief Initialize the I2C peripheral
 *
 * @param init values for initialization parameters
 */
void I2C_init(I2C_Init_Typedef *init);

/**
 * @brief Set the I2C bitrate in KHz
 *
 * @param bitRateKhz bitrate in KHz from 10 to 400
 */
void I2C_setBitRate(uint16_t bitRateKhz);

/**
 * @brief Write an array of bytes
 *
 * @param targetAddr 7-bit peripheral address
 * @param writeData array of bytes to be written
 * @param nWriteBytes number of bytes to be written
 * @return uint8_t I2C status code result
 */
uint8_t I2C_writeBytes(const uint8_t targetAddr, const uint8_t *writeData,
                       const uint8_t nWriteBytes);

/**
 * @brief Read an array of bytes
 *
 * @param targetAddr 7-bit peripheral address
 * @param readData array of bytes to be read
 * @param nReadBytes number of bytes to be read
 * @return uint8_t I2C status code result
 */
uint8_t I2C_readBytes(const uint8_t targetAddr, uint8_t *readData,
                      const uint8_t nReadBytes);

/**
 * @brief write an array of bytes then read an array of bytes
 *
 * @param targetAddr 7-bit peripheral address
 * @param writeData array of bytes to be written
 * @param nWriteBytes number of bytes to be written
 * @param readData array of bytes to be read
 * @param nReadBytes number of bytes to be read
 * @return uint8_t I2C status code result
 */
uint8_t I2C_writeReadBytes(const uint8_t targetAddr, const uint8_t *writeData,
                           const uint8_t nWriteBytes, uint8_t *readData,
                           const uint8_t nReadBytes);

/**
 * @brief Scan through possible addresses and identify addresses that respond
 *
 * @param addrList array to store addresses found
 * @param maxLength size of array
 * @return uint8_t number of addresses found
 */
uint8_t I2C_scanForDevices(uint8_t *addrList, const uint8_t maxLength);

#endif // TWCR

#endif /* I2C_H */