#include "global.h"
#include <i2c.h>
#include <stdio.h>
#include <uart.h>
#include <util/delay.h>

#define LM75_ADDRESS 0b1001000
#define LM75_TEMP_REGISTER 0b00000000

int8_t read_Temp8b()
{
  uint8_t writeData[] = {LM75_TEMP_REGISTER}, readData[2];
  uint8_t status = I2C_writeReadBytes(LM75_ADDRESS,
                                      writeData, 1, readData, 2);
  printf("status=0x%02x\n", status);
  return (int8_t)readData[0];
}

int main(void)
{
  UART_init();
  I2C_Init_Typedef init = {
      .bitRateSelect = 32,
      .clkSelect = 0,
      .interuptEn = false};
  I2C_init(&init);
  I2C_setBitRate(400);

  uint8_t addrs[5], n_found;
  n_found = I2C_scanForDevices(addrs, 5);
  printf("[");
  for (uint8_t i = 0; i < n_found; i++)
  {
    printf("%d, ", addrs[i]);
  }
  printf("]\n");

  for (;;)
  {
    printf("temp=%d\n", read_Temp8b());
    _delay_ms(1000);
  }
  return 0;
}
