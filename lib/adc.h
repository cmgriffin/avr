#ifndef __ADC__
#define __ADC__

#include "avrlibdefs.h"

#define ADC_TEMP_GAIN   82
#define ADC_TEMP_OFFSET 26583

// ADMUX DEFINES
#define ADC_ADMUX_MUX_INPUT0 0x0
#define ADC_ADMUX_MUX_INPUT1 0x1
#define ADC_ADMUX_MUX_INPUT2 0x2
#define ADC_ADMUX_MUX_INPUT3 0x3
#define ADC_ADMUX_MUX_INPUT4 0x4
#define ADC_ADMUX_MUX_INPUT5 0x5
#define ADC_ADMUX_MUX_INPUT6 0x6
#define ADC_ADMUX_MUX_INPUT7 0x7
#define ADC_ADMUX_MUX_TEMP   0x8
#define ADC_ADMUX_MUX_REF    0xe
#define ADC_ADMUX_MUX_GND    0xf
#define ADC_ADMUX_MUX_MASK   0xf

#define ADC_ADMUX_LSHIFT     (0x1 << ADLAR)

#define ADC_ADMUX_REF_EXT    (0x0 << REFS0)
#define ADC_ADMUX_REF_VCC    (0x1 << REFS0)
#define ADC_ADMUX_REF_INT    (0x3 << REFS0)
#define ADC_ADMUX_REF_MASK   (0x3 << REFS0)

// ADSRA DEFINES
#define ADC_ADCSRA_CLK_DIV2      0x0
#define ADC_ADCSRA_CLK_DIV4      0x2
#define ADC_ADCSRA_CLK_DIV8      0x3
#define ADC_ADCSRA_CLK_DIV16     0x4
#define ADC_ADCSRA_CLK_DIV32     0x5
#define ADC_ADCSRA_CLK_DIV64     0x6
#define ADC_ADCSRA_CLK_DIV128    0x7

#define ADC_ADCSRA_INTERUPT_EN   _BV(ADIE)
#define ADC_ADCSRA_INTERUPT_FLAG _BV(ADIF)
#define ADC_ADCSRA_AUTO_TRIG_EN  _BV(ADATE)
#define ADC_ADCSRA_START_CONV    _BV(ADSC)
#define ADC_ADCSRA_ADC_EN        _BV(ADEN)

// ADSRB DEFINES
#define ADC_ADCSRB_TRG_FREE  0x0
#define ADC_ADCSRB_TRG_COMP  0x1
#define ADC_ADCSRB_TRG_INT0  0x2
#define ADC_ADCSRB_TRG_OCR0A 0x3
#define ADC_ADCSRB_TRG_TOV0  0x4
#define ADC_ADCSRB_TRG_OCR1B 0x5
#define ADC_ADCSRB_TRG_TOV1  0x6
#define ADC_ADCSRB_TRG_ICR1  0x7

void ADC_init(const uint8_t mux, const uint8_t ref, const uint8_t clk_div);

uint16_t ADC_measure(uint8_t mux);
uint16_t ADC_measure_current();

int8_t ADC_measure_temp();

#endif // __ADC__