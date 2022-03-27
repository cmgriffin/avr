#include "devices/lm75.h"
#include <i2c.h>

int8_t LM75_measure_temp_8b(uint8_t addr)
{
    uint8_t writeData[] = {LM75_REG_TEMP}, readData[2];
    uint8_t status = I2C_writeReadBytes(addr,
                                        writeData, 1, readData, 2);
    if (status != I2C_STATUS_OK)
    {
        return -128;
    }
    return (int8_t)readData[0];
}

int16_t LM75_measure_temp_9b(uint8_t addr)
{
    uint8_t writeData[] = {LM75_REG_TEMP}, readData[2];
    uint8_t status = I2C_writeReadBytes(addr,
                                        writeData, 1, readData, 2);
    if (status != I2C_STATUS_OK)
    {
        return -128;
    }
    return (int16_t)(((uint16_t)readData[0] << 1) + ((readData[1] & 0x80) ? 1 : 0));
}
