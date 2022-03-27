#include "global.h"
#include <devices/lm75.h>
#include <i2c.h>
#include <stdio.h>
#include <uart.h>
#include <util/delay.h>

int main(void)
{
  UART_init();
  I2C_Init_Typedef init = {
      .bitRateSelect = 32,
      .clkSelect = 0,
      .interuptEn = false};
  I2C_init(&init);
  I2C_setBitRate(400);

  // uint8_t addrs[5], n_found;
  // n_found = I2C_scanForDevices(addrs, 5);
  // printf("[");
  // for (uint8_t i = 0; i < n_found; i++)
  // {
  //   printf("%d, ", addrs[i]);
  // }
  // printf("]\n");

  for (;;)
  {
    int16_t raw = LM75_measure_temp_9b(LM75_ADDR_BASE);
    int8_t whole = (int8_t)(raw >> 1);
    char dec = (raw & 0x1) ? '5' : '0';
    printf("raw=%d, temp=%d.%c\n", raw, whole, dec);
    _delay_ms(100);
  }
  return 0;
}
