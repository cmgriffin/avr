#include "global.h"
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <uart.h>

int main(void)
{
  UART_init();
  char read_str[10];
  char *rxptr;

  sei();
  for (;;)
  {
    rxptr = read_str;

    UART_printStr_p(PSTR("Enter a string: "));
    while (UART_readLine(read_str, 10, &rxptr) == NULL)
    {
    }
    UART_printStr_p(PSTR("You entered: "));
    UART_printStr(read_str);
    UART_printStr_p(PSTR("\n"));
  }
  return 0;
}
