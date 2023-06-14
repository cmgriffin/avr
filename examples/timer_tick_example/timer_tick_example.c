#include "global.h"
#include <avr/interrupt.h>
#include <gpio.h>
#include <stddef.h>
#include <stdio.h>
#include <timer.h>
#include <uart.h>
#include <util/delay.h>

const GPIO_TypeDef led = D13;

void toggleLed()
{
  GPIO_toggleValue(&led);
}

int main(void)
{
  GPIO_setOutput(&led);
  TIMER_tick_init(TIMER0_CLK_DIV64); // tick frequency = fcpu / 1024 / 256
  TIMER_attach_tick_func(toggleLed); // attach a simple
  UART_init();
  sei();
  printf("current ms = %d\n", TIMER_time_ms());
  TIMER_delay_ms(1000);
  printf("current ms = %d\n", TIMER_time_ms());

  uint16_t timer = TCNT0;
  //_delay_us(100);
  timer = TCNT0 - timer;
  printf("10us delay took = %d timer counts\n", timer);

  for (;;)
  {
    //_delay_ms(500);
  }
  return 0;
}
