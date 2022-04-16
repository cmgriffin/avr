#include "global.h"
#include <devices/ds18b20.h>
#include <gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <uart.h>
#include <util/delay.h>

void print_roms(DS18B20_rom_t *roms, uint8_t len)
{
    for (uint8_t iRom = 0; iRom < len; iRom++)
    {
        printf("[");
        for (uint8_t b = 0; b < 8; b++)
        {
            printf("%X, ", roms[iRom].x[b]);
        }
        printf("]\n");
    }
}

void print_temps(GPIO_TypeDef *pin, DS18B20_rom_t *roms, uint8_t len)
{
    int32_t temp;
    printf("[");
    for (uint8_t iRom = 0; iRom < len; iRom++)
    {

        DS18B20_readTemp(pin, &roms[iRom], &temp, 1);
        printf("%ld, ", temp);
    }
    printf("]\n");
}

int main(void)
{
    GPIO_TypeDef ds_pin = A5;
    UART_init();

    DS18B20_rom_t device_roms[6];

    uint8_t devices_found =
        DS18B20_search(&ds_pin, DS18B20_CMD_SEARCH_ROM, device_roms, 6);

    printf("search result = %d\n", devices_found);
    print_roms(device_roms, devices_found);

    for (;;)
    {
        DS18B20_setConfig(&ds_pin, NULL, -20, 30, DS18B20_RESOLUTION_9b);
        DS18B20_startConversion(&ds_pin);
        while (!DS18B20_isComplete(&ds_pin))
        {
        }
        int32_t temp;
        DS18B20_readTemp(&ds_pin, NULL, &temp, 4);
        printf("%ld\n", temp);
        // print_temps(&ds_pin, device_roms, devices_found);
    }
    return 0;
}
