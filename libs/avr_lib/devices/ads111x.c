#include "devices/ads111x.h"

#include <i2c.h>

uint8_t ADS111X_writeReg(uint8_t i2cAddr, uint8_t regAddr, uint16_t val)
{
    uint8_t writeData[] = {regAddr, (uint8_t)(val >> 8), (uint8_t)val};
    uint8_t status      = I2C_writeBytes(i2cAddr, writeData, 3);
    return status;
}
uint8_t ADS111X_readReg(uint8_t i2cAddr, uint8_t regAddr, uint16_t *readVal)
{
    uint8_t writeData[] = {regAddr};
    uint8_t readData[2];
    uint8_t status = I2C_writeReadBytes(i2cAddr, writeData, 1, readData, 2);
    *readVal       = ((uint16_t)readData[0] << 8) + readData[1];
    return status;
}

void ADS111X_updateConfig(uint8_t i2cAddr, uint16_t config_val,
                          uint16_t config_mask)
{
    uint16_t config;
    ADS111X_readReg(i2cAddr, ADS111X_REG_CONFIG, &config);
    update_bits(config, config_val, config_mask);
    ADS111X_writeReg(i2cAddr, ADS111X_REG_CONFIG,
                     config & ~ADS111X_CONFIG_OS_MASK);
}

void ADS111X_startConversion(uint8_t i2cAddr)
{
    uint16_t config;
    ADS111X_readReg(i2cAddr, ADS111X_REG_CONFIG, &config);
    ADS111X_writeReg(i2cAddr, ADS111X_REG_CONFIG,
                     (config | ADS111X_CONFIG_OS_MASK));
}

bool ADS111X_conversionComplete(uint8_t i2cAddr)
{
    uint16_t val;
    ADS111X_readReg(i2cAddr, ADS111X_REG_CONFIG, &val);
    return !!(val & ADS111X_CONFIG_OS_MASK);
}

int16_t ADS111X_measureCurrent(uint8_t i2cAddr)
{
    ADS111X_startConversion(i2cAddr);
    while (!ADS111X_conversionComplete(i2cAddr))
    {
    }
    uint16_t val;
    ADS111X_readReg(i2cAddr, ADS111X_REG_CONV, &val);
    return (int16_t)val;
}

int16_t ADS111X_fetchCurrent(uint8_t i2cAddr)
{
    uint16_t val;
    ADS111X_readReg(i2cAddr, ADS111X_REG_CONV, &val);
    return (int16_t)val;
}

double ADS111X_measureVolts(uint8_t i2cAddr, double fsrVolts)
{
    return (double)ADS111X_measureCurrent(i2cAddr) * fsrVolts /
           (double)(1 << 15);
}
