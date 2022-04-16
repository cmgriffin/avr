#include "devices/ds18b20.h"
#include <gpio.h>
#include <util/crc16.h>
#include <util/delay.h>

#define DEBUG_SHORT
#include <debug.h>

// Private

// ROM Commands
#define __CMD_SEARCH_ROM   0xF0
#define __CMD_READ_ROM     0x33
#define __CMD_MATCH_ROM    0x55
#define __CMD_SKIP_ROM     0xCC
#define __CMD_ALARM_SEARCH 0xEC
// Function Commands
#define __CMD_CONVERT_T         0x44
#define __CMD_WRITE_SCRATCHPAD  0x4E
#define __CMD_READ_SCRATCHPAD   0xBE
#define __CMD_COPY_SCRATCHPAD   0x48
#define __CMD_RECALL_E2         0xB8
#define __CMD_READ_POWER_SUPPLY 0xB4

typedef enum
{
    iTEMP_LSB = 0,
    iTEMP_MSB = 1,
    iTH_REG   = 2,
    iTL_REG   = 3,
    iCONF_REG = 4,
    iCRC      = 8
} __scratchpadEnum_t;

typedef struct
{
    uint8_t x[9];
} __scratchpad_t;

void __writeByte(GPIO_TypeDef *pin, uint8_t byte)
{
    for (uint8_t i = 0, mask = 1; i < 8; mask <<= 1, i++)
    {
        GPIO_setOutput(pin);
        GPIO_setValueLow(pin);
        if (byte & mask)
            _delay_us(1);
        else
            _delay_us(60);
        GPIO_setInput(pin);
        if (byte & mask)
            _delay_us(60);
        else
            _delay_us(1);
    }
}

uint8_t __readByte(GPIO_TypeDef *pin)
{
    uint8_t byte = 0;
    for (uint8_t i = 0, mask = 1; i < 8; mask <<= 1, i++)
    {
        GPIO_setOutput(pin);
        GPIO_setValueLow(pin);
        _delay_us(1);
        GPIO_setInput(pin);
        _delay_us(15);
        if (GPIO_getInput(pin))
            byte += mask;
        _delay_us(44);
    }
    return byte;
}

void __writeBit(GPIO_TypeDef *pin, uint8_t bit)
{

    GPIO_setOutput(pin);
    GPIO_setValueLow(pin);
    if (bit)
        _delay_us(1);
    else
        _delay_us(60);
    GPIO_setInput(pin);
    if (bit)
        _delay_us(60);
    else
        _delay_us(1);
}

uint8_t __readBit(GPIO_TypeDef *pin)
{
    uint8_t bit = 0;

    GPIO_setOutput(pin);
    GPIO_setValueLow(pin);
    _delay_us(1);
    GPIO_setInput(pin);
    _delay_us(15);
    if (GPIO_getInput(pin))
        bit = 1;
    _delay_us(44);
    return bit;
}

bool __reset(GPIO_TypeDef *pin)
{
    GPIO_setOutput(pin);
    GPIO_setValueLow(pin);
    _delay_us(500);
    GPIO_setInput(pin);
    _delay_us(60);
    bool presence = !GPIO_getInput(pin);
    _delay_us(440);
    return presence;
}

static inline int16_t __intPow(int8_t base, int8_t exp)
{
    int16_t result = 1;
    for (; exp; --exp)
    {
        result *= base;
    }
    return result;
}

int32_t __calcTemp(uint8_t l, uint8_t h, uint8_t exp)
{
    int16_t raw = (int16_t)((uint16_t)l + ((uint16_t)h << 8));
    // clear the unused bits

    if (exp < 4)
    {
        for (uint8_t i = _BV(3 - exp); i; i >>= 1)
        {
            raw &= ~i;
        }
    }
    return (int32_t)raw * __intPow(5, 4);
}

bool __writeScratchpad(GPIO_TypeDef *pin, uint8_t high, uint8_t low,
                       uint8_t config)
{
    __writeByte(pin, __CMD_WRITE_SCRATCHPAD);
    __writeByte(pin, high);
    __writeByte(pin, low);
    __writeByte(pin, config);
    return true;
}

bool __readScratchpad(GPIO_TypeDef *pin, __scratchpad_t *scratch)
{
    // bool crcXor = false;
    uint8_t byte, crc = 0;

    // read in the data from the scratchpad
    __writeByte(pin, __CMD_READ_SCRATCHPAD);
    for (byte = 0; byte < 9; byte++)
    {
        scratch->x[byte] = __readByte(pin);
    }

    for (byte = 0; byte < 8; byte++)
    {
        crc = _crc_ibutton_update(crc, scratch->x[byte]);
    }
    if (crc == scratch->x[iCRC])
        return true;
    return false;
}

// Public Functions

bool DS18B20_startConversion(GPIO_TypeDef *pin)
{
    if (!__reset(pin))
        return false;
    __writeByte(pin, __CMD_SKIP_ROM);
    __writeByte(pin, __CMD_CONVERT_T);
    return true;
}

bool DS18B20_isComplete(GPIO_TypeDef *pin) { return __readBit(pin); }

bool DS18B20_singleReadRom(GPIO_TypeDef *pin, DS18B20_rom_t *rom)
{

    uint8_t i;

    if (!__reset(pin))
        return false;
    __writeByte(pin, __CMD_READ_ROM);
    for (i = 0; i < 8; i++)
    {
        rom->x[i] = __readByte(pin);
    }

    return true;
}

bool DS18B20_readTemp(GPIO_TypeDef *pin, DS18B20_rom_t *rom, int32_t *temp,
                      uint8_t exp)
{

    if (!DS18B20_isComplete(pin))
    {
        return false;
    }
    __reset(pin);
    if (rom == NULL)
    {
        __writeByte(pin, __CMD_SKIP_ROM);
    }
    else
    {
        __writeByte(pin, __CMD_MATCH_ROM);
        for (uint8_t i = 0; i < 8; i++)
        {
            __writeByte(pin, rom->x[i]);
        }
    }

    __scratchpad_t scratch;
    __readScratchpad(pin, &scratch);
    *temp = __calcTemp(scratch.x[iTEMP_LSB], scratch.x[iTEMP_MSB], exp);
    return true;
}

bool DS18B20_readTempBlocking(GPIO_TypeDef *pin, DS18B20_rom_t *rom,
                              int32_t *temp, uint8_t exp)
{

    if (!DS18B20_startConversion(pin))
    {
        return false;
    }
    while (!DS18B20_isComplete(pin))
    {
    }

    return DS18B20_readTemp(pin, rom, temp, exp);
}

bool DS18B20_setConfig(GPIO_TypeDef *pin, DS18B20_rom_t *rom, int8_t high,
                       int8_t low, uint8_t config)
{
    __reset(pin);
    if (rom == NULL)
    {
        __writeByte(pin, __CMD_SKIP_ROM);
    }
    else
    {
        __writeByte(pin, __CMD_MATCH_ROM);
        for (uint8_t i = 0; i < 8; i++)
        {
            __writeByte(pin, rom->x[i]);
        }
    }

    __writeScratchpad(pin, (uint8_t)high, (uint8_t)low, config | 0x1f);
    return true;
}

uint8_t DS18B20_search(GPIO_TypeDef *pin, uint8_t searchCmd,
                       DS18B20_rom_t *romList, uint8_t maxLen)
{

    uint8_t nDevice = 0; // current number of the device being enumerated
    // lastDescrepancy that occured from previous device
    uint8_t lastDescrepancy = 0;
    bool searchDone         = false; // indicate when the search is finished
    DS18B20_rom_t curr_rom;          // holds the current rom we are enumerating
    // make sure it starts as all 0s
    for (uint8_t i = 0; i < 8; i++)
    {
        curr_rom.x[i] = 0;
    }
    while (nDevice < maxLen)
    {
        uint8_t rom_bit_index     = 0; // rom bit number (0 to 63)
        uint8_t descrepancyMarker = 0; // current device last descrepancy index
        uint8_t id_bit, id_bit_cmp;    // id_bit and the id_bit_complement

        if (searchDone)
        {
            break;
        }

        if (!__reset(pin))
        {
            // no presence
            lastDescrepancy = 0;
            break;
        }
        __writeByte(pin, searchCmd);
        for (rom_bit_index = 0; rom_bit_index < 64; rom_bit_index++)
        {
            id_bit     = __readBit(pin);
            id_bit_cmp = __readBit(pin);
            // compute these to allow easier access to the arrays
            uint8_t byte_mask   = _BV(rom_bit_index % 8);
            uint8_t byte_number = rom_bit_index / 8;

            // Determine which direction to search based on the response
            if (id_bit && id_bit_cmp)
            {
                lastDescrepancy = 0;
                break;
            }
            else
            {
                if (!id_bit && !id_bit_cmp)
                {
                    // descrepancy occured
                    if (rom_bit_index == lastDescrepancy)
                    {
                        curr_rom.x[byte_number] |= byte_mask;
                    }
                    else
                    {
                        if (rom_bit_index > lastDescrepancy)
                        {
                            curr_rom.x[byte_number] &= ~byte_mask;
                            descrepancyMarker = rom_bit_index;
                        }
                        else
                        {
                            if (!(curr_rom.x[byte_number] & byte_mask))
                            {
                                descrepancyMarker = rom_bit_index;
                            }
                        }
                    }
                }
                else
                {
                    // no descrepancy, set to id_bit value
                    if (id_bit)
                    {
                        curr_rom.x[byte_number] |= byte_mask;
                    }
                    else
                    {
                        curr_rom.x[byte_number] &= ~byte_mask;
                    }
                }
            }

            // finally write the bit we decided on to continue the search
            __writeBit(pin, curr_rom.x[byte_number] & byte_mask);
        } // single device enumeration loop

        if (!(rom_bit_index < 64))
        {
            // successfully enumerated a device
            lastDescrepancy = descrepancyMarker;
            if (lastDescrepancy == 0)
            {
                searchDone = true;
            }
            // copy over the rom
            for (uint8_t i = 0; i < 8; i++)
            {
                romList[nDevice].x[i] = curr_rom.x[i];
            }
            // go to the next device
            nDevice++;
        }
        else
        {
            break;
        }
    } // all devices enumeration loop

    return nDevice;
}
