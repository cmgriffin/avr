#include "adc.h"
#include "avrlibdefs.h"
#include "global.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <debug.h>
#include <util/delay.h>

void ADC_init(
    const uint8_t mux,
    const uint8_t ref,
    const uint8_t clk_div)
{
    ADMUX = mux | ref;
    ADCSRA = clk_div | ADC_ADCSRA_ADC_EN;
#ifdef ADC_USE_SLEEP_MODE
    set_sleep_mode(SLEEP_MODE_ADC);
    ADCSRA |= _BV(ADIE);
    sei();
#endif
}

#ifdef ADC_USE_SLEEP_MODE
EMPTY_INTERRUPT(ADC_vect);
#endif

uint16_t ADC_measure_current()
{
#ifdef ADC_USE_SLEEP_MODE
    sleep_mode();
#else
    ADCSRA |= _BV(ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);
#endif
    uint16_t counts = ADC;
#ifdef ADC_DEBUG
    _DEBUG("%d", counts);
#endif
    return counts;
}

uint16_t ADC_measure(uint8_t mux)
{
    update_bits(ADMUX, mux, ADC_ADMUX_MUX_MASK);
    return ADC_measure_current();
}

int8_t ADC_measure_temp()
{
    uint8_t admux_prev = ADMUX;
    update_bits(ADMUX, ADC_ADMUX_REF_INT | ADC_ADMUX_MUX_TEMP, ADC_ADMUX_MUX_MASK | ADC_ADMUX_REF_MASK);
    _delay_ms(ADC_REF_SETTLE_TIME);
    int16_t raw = (int16_t)ADC_measure_current();
    ADMUX = admux_prev;
    _delay_ms(ADC_REF_SETTLE_TIME);
    return (int8_t)((ADC_TEMP_GAIN * raw - ADC_TEMP_OFFSET) / 100);
}