#include "global.h"
#include <avr/interrupt.h>
#include <gpio.h>
#include <stddef.h>
#include <stdio.h>
#include <timer.h>
#include <uart.h>
#include <util/delay.h>
#include <avr/interrupt.h>

const GPIO_TypeDef led = D13;

ISR(TIMER0_OVF_vect)
{
  // turn on
  GPIO_setValueHigh(&led);
}
ISR(TIMER0_COMPA_vect)
{
  // turn off
  GPIO_setValueLow(&led);
}

int main(void)
{

  GPIO_setOutput(&led);

  const Timer_Init_Typedef init = {
      .clockSelect = TIMER0_CLK_DIV256, // ~60Hz PWM
      .ocConfig = TIMER_OCA_CLEAR,
      .wgmConfig = TIMER0_WGM_PWM_FAST_FF,
      .interuptEnable = TIMER_INTERUPT_TOV | TIMER_INTERUPT_OCA};
  TIMER0_init(&init, false);
  bool increasing = true;
  OCR0A = 0;
  sei();
  // GPIO_setValueHigh(&led);
  for (;;)
  {
    if (OCR0A == 0xff && increasing)
    {
      increasing = false;
    }
    if (OCR0A == 0 && !increasing)
    {
      increasing = true;
    }

    if (increasing)
    {
      OCR0A++;
    }
    else
    {
      OCR0A--;
    }

    _delay_ms(2);

    // GPIO_toggleValue(&led);
    // if (OCR0A == 0xff)
    // {
    //   OCR0A = 1;
    // }
    // _delay_ms(10);
    // OCR0A += 0x1;
  }
  return 0;
}
