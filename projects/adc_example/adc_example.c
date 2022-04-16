#include "global.h"
#include <adc.h>

#include <avr/pgmspace.h>
#include <uart.h>
#include <util/delay.h>

int main(void)
{
  UART_init();
  ADC_init(ADC_ADMUX_MUX_INPUT0, ADC_ADMUX_REF_VCC, ADC_ADCSRA_CLK_DIV128);
  // ADMUX = ADC_ADMUX_MUX_TEMP | ADC_ADMUX_REF_INT;
  // ADCSRA = ADC_ADCSRA_CLK_DIV128 | ADC_ADCSRA_ADC_EN;
  for (;;)
  {
    UART_printStr_p(PSTR("temp measured = "));
    UART_print_i16((int16_t)ADC_measure_temp(), 0, true);
    UART_printStr_p(PSTR(", input measured = "));
    UART_print_u16(ADC_measure(ADC_ADMUX_MUX_INPUT0), 0, 0, 0);
    UART_TransmitByte('\n', true);
    _delay_ms(500);
  }
  return 0;
}
