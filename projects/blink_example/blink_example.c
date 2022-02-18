#include "global.h"
#include <util/delay.h>
#include <gpio.h>

int main(void)
{
  GPIO_TypeDef led = D13;
  GPIO_setOutput(&led);

  for (;;)
  {
    GPIO_toggleValue(&led);
    _delay_ms(1000);
  }
  return 0;
}
