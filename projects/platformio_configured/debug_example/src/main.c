#include <global.h>
#include <debug.h>
#include <stdio.h>
#include <uart.h>
#include <util/delay.h>

int main(void)
{
  UART_init();

  int count = 0;
  for (;;)
  {
    _DEBUG("debug test bare AVR %d", count);
    _delay_ms(1000);
    count++;
  }

  return 0;
}
