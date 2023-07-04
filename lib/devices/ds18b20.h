#ifndef __DS18B20__
#define __DS18B20__

#include <avrlibdefs.h>
#include <gpio.h>

// ROM Commands necessary for public interface
#define DS18B20_CMD_SEARCH_ROM   0xF0
#define DS18B20_CMD_ALARM_SEARCH 0xEC

#define DS18B20_RESOLUTION_12b   (0x3 << 5)
#define DS18B20_RESOLUTION_11b   (0x2 << 5)
#define DS18B20_RESOLUTION_10b   (0x1 << 5)
#define DS18B20_RESOLUTION_9b    (0x0 << 5)

/**
 * @brief structure to hold a single DS18B20 ROM value
 *
 */
typedef struct
{
    uint8_t x[8];
} DS18B20_rom_t;

// start the temperature conversion then return
bool DS18B20_startConversion(GPIO_TypeDef *pin);

// read the rom address for when a single device is connectd to the bus
bool DS18B20_singleReadRom(GPIO_TypeDef *pin, DS18B20_rom_t *rom);

bool DS18B20_isComplete(GPIO_TypeDef *pin);

uint8_t DS18B20_search(GPIO_TypeDef *pin, uint8_t searchCmd,
                       DS18B20_rom_t *romList, uint8_t maxLen);

bool DS18B20_readTempValue(GPIO_TypeDef *pin, DS18B20_rom_t *rom,
                           int16_t *temp);

bool DS18B20_readTemp(GPIO_TypeDef *pin, DS18B20_rom_t *rom, int32_t *temp,
                      uint8_t exp);

bool DS18B20_readTempBlocking(GPIO_TypeDef *pin, DS18B20_rom_t *rom,
                              int32_t *temp, uint8_t exp);

bool DS18B20_setConfig(GPIO_TypeDef *pin, DS18B20_rom_t *rom, int8_t high,
                       int8_t low, uint8_t config);

#endif // __DS18B20__