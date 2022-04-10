#include "global.h"
#include <devices/ads111x.h>
#include <i2c.h>
#include <stdio.h>
#include <stdlib.h>
#include <uart.h>
#include <util/delay.h>

int main(void)
{
    UART_init();
    I2C_Init_Typedef init = {
        .bitRateSelect = 32, .clkSelect = 0, .interuptEn = false};
    I2C_init(&init);
    I2C_setBitRate(400);

    ADS111X_updateConfig(72,
                         ADS111X_CONFIG_PGA_4096FSR | ADS111X_CONFIG_DR_860SPS,
                         ADS111X_CONFIG_PGA_MASK | ADS111X_CONFIG_DR_MASK);
    uint16_t config;
    ADS111X_readReg(72, ADS111X_REG_CONFIG, &config);
    printf("config reg = 0x%x\n", config);
    char floatStr[12];
    for (;;)
    {
        printf("[");
        for (uint8_t i = 0; i < 8; i++)
        {
            ADS111X_updateConfig(72, (i << 12), ADS111X_CONFIG_MUX_MASK);
            // printf("%d, ", ADS111X_measureCurrent(72));
            dtostrf(ADS111X_measureVolts(72, 4.096), 0, 6, floatStr);
            printf("%s, ", floatStr);
        }

        printf("]\n");
        _delay_ms(1000);
    }
    return 0;
}
