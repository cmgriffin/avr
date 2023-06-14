#include "global.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <uart.h>

int main(void)
{
  UART_init();
  char read_str[10];
  sei();
  for (;;)
  {
    printf_P(PSTR("Enter a string: "));
    scanf_P(PSTR("%9s"), read_str);
    printf_P(PSTR("You entered: %s\n"), read_str);
  }
  return 0;
}
