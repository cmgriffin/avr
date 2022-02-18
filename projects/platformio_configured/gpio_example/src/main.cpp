#include <avr/io.h>
#include <global.h>
#include <gpio.h>
#include <uart.h>
#include <util/delay.h>

int main(void)
{
  GPIO_TypeDef led_pin = D13;
  UART_init();
  UART_printStr_p(PSTR("Program Start!\n"));
  GPIO_setOutput(&led_pin);

  for (;;)
  {
    GPIO_toggleValue(&led_pin);
    UART_printStr_p(PSTR("Led is "));
    if (GPIO_getInput(&led_pin))
    {
      UART_printStr_p(PSTR("ON\n"));
    }
    else
    {
      UART_printStr_p(PSTR("OFF\n"));
    }
    _delay_ms(1000);
  }

  return 0;
}