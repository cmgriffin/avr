#include "global.h"
#include <avr/pgmspace.h>
#include <buffer.h>
#include <stdio.h>
#include <uart.h>

void print_array(uint8_t *arr, uint8_t len)
{
  printf_P(PSTR("[ "));
  for (uint8_t i = 0; i < len; i++)
  {
    printf_P(PSTR("%d, "), arr[i]);
  }
  printf_P(PSTR("]\n"));
}

int main(void)
{
  UART_init();
  uint8_t buff[20];
  for (uint8_t i = 0; i < sizeof(buff); i++)
  {
    buff[i] = 0;
  }

  buffer_t buffer = BUFFER_CREATE(20, buff);

  for (uint8_t i = 0; i < 10; i++)
  {
    BUFFER_enqueue(&buffer, i);
  }

  print_array(buff, 20);

  for (uint8_t i = 0; i < 20; i++)
  {
    printf_P(PSTR("%d - %d\n"), i, BUFFER_dequeue(&buffer));
  }

  for (uint8_t i = 0; i < 5; i++)
  {
    BUFFER_enqueue(&buffer, 30 + i);
  }
  print_array(buff, 20);

  for (uint8_t i = 0; i < 20; i++)
  {
    printf_P(PSTR("%d - %d\n"), i, BUFFER_dequeue(&buffer));
  }
  for (;;)
  {
  }
  return 0;
}
