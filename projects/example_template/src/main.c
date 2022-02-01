#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <uart.h>

#ifndef LED_PIN
#define LED_PIN PB5
#endif

int main(void)
{
  UART_init();
  char buf[32];
  uint16_t addr = 0xAAAA;
  for (uint16_t i = 0x8000; i; i >>= 1)
  {
    uint16_t anded = addr & i;
    uint16_t logical = !!anded;
    sprintf_P(buf, PSTR("%04x %04x %04x\n"), i, anded, logical);
    UART_printStr(buf);
  }

  for (;;)
  {
  }

  return 0;
}