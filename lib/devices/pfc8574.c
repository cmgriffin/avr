#include "devices/pfc8574.h"
#include <i2c.h>
#include <util/delay.h>

void PFC8574_write(uint8_t addr, uint8_t data)
{
    uint8_t _data[1] = {data};
    I2C_writeBytes(addr, _data, 1);
    _delay_us(5); // enforce a 5us bus idle time prior to another start conditon
}

uint8_t PFC8574_read(uint8_t addr)
{
    uint8_t data;
    I2C_readBytes(addr, &data, 1);
    _delay_us(5); // enforce a 5us bus idle time prior to another start conditon
    return data;
}