#include "global.h"
#include <avr/pgmspace.h>
#include <buffer.h>
#include <stdio.h>
#include <uart.h>

void print_array(uint8_t *arr, uint8_t len)
{
  printf_P(PSTR("\tarray=[ "));
  for (uint8_t i = 0; i < len; i++)
  {
    printf_P(PSTR("%d, "), arr[i]);
  }
  printf_P(PSTR("]\n"));
}

void print_buffer_state(buffer_t *b)
{
  printf_P(
      PSTR("buffer(\n\thead=%d, tail=%d, num_entries=%d, size=%d, "),
      b->head,
      b->tail, b->num_entries, b->size);
  printf_P(PSTR("full=%d, empty=%d, \n"), BUFFER_full(b), BUFFER_empty(b));
  print_array(b->valuesptr, b->size);
  printf_P(PSTR(")\n"));
}

int main(void)
{
  UART_init();
  printf_P(PSTR("\n\n"));
  uint8_t buff[20]; // array for the buffer to use
  // initialize to 0 (un-needed but better for printing)
  for (uint8_t i = 0; i < sizeof(buff); i++)
  {
    buff[i] = 0;
  }
  buffer_t buffer = BUFFER_CREATE(20, buff);
  print_buffer_state(&buffer);

  // fill the buffer
  for (uint8_t i = 0; i < 20; i++)
  {
    BUFFER_enqueue(&buffer, i);
  }
  print_buffer_state(&buffer);

  for (uint8_t i = 0; i < 20; i++)
  {
    printf_P(PSTR("%d - %d\n"), i, BUFFER_dequeue(&buffer));
  }

  print_buffer_state(&buffer);

  for (;;)
  {
  }
  return 0;
}
