#if __has_include("timer_conf.h")
#include "timer_conf.h"

#include "timer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "debug.h"
#include "global.h"

void TIMER_init(const Timer_Init_Typedef *init, bool clearFirst,
                volatile uint8_t *const tccra, volatile uint8_t *const tccrb,
                volatile uint8_t *const timsk)
{
    if (clearFirst)
    {
        *tccra = 0;
        *tccrb = 0;
        *timsk = 0;
    }
    // configure the compare outputs per init and use fast PWM mode with top of
    // 0xff
    *tccra |= init->ocConfig | (init->wgmConfig & 0x03);
    // setup the clock source and prescaler
    *tccrb |= init->clockSelect | ((init->wgmConfig & 0xFC) << 1);
    *timsk |= init->interuptEnable;
}

#ifdef TCCR0A
void TIMER0_init(const Timer_Init_Typedef *init, bool clearFirst)
{
    TIMER_init(init, clearFirst, &TCCR0A, &TCCR0B, &TIMSK0);
}
#endif // TCCR0A

#ifdef TCCR1A
void TIMER1_init(const Timer_Init_Typedef *init, bool clearFirst)
{
    TIMER_init(init, clearFirst, &TCCR1A, &TCCR1B, &TIMSK1);
}
#endif // TCCR1A

#ifdef TCCR2A
void TIMER2_init(const Timer_Init_Typedef *init, bool clearFirst)
{
    TIMER_init(init, clearFirst, &TCCR2A, &TCCR2B, &TIMSK2);
}
#endif // TCCR2A

// continuously updated to track current number of ticks since rollover
// needs to be volatile since it is only updated in the ISR
static volatile uint16_t current_ticks;

static void (*tick_func)();

uint16_t TIMER_getTicks()
{
    uint16_t copy;
    ATOMIC_BLOCK(ATOMIC_FORCEON) { copy = current_ticks; }
    return copy;
}

void TIMER_delayTicks(uint16_t ticks)
{
    uint16_t start;
    start = TIMER_getTicks();

    do
    {
    } while (TIMER_getTicks() - start < ticks);
}

void TIMER_delay_ms(uint16_t ms)
{
    uint16_t ticks = (uint16_t)(((uint32_t)ms * (F_CPU / 1000UL) +
                                 (TIMER_TICK_CLK_DIV >> 1)) /
                                TIMER_TICK_CLK_DIV);
    _DEBUG("ms=%d, ticks=%d", ms, ticks);
    TIMER_delayTicks(ticks);
}

uint16_t TIMER_time_ms()
{
    uint32_t ticks = (uint32_t)TIMER_getTicks();
    _DEBUG("current ticks=%d", ticks);
    return (uint16_t)((TIMER_TICK_CLK_DIV * ticks + (F_CPU / 2000)) /
                      (F_CPU / 1000));
}

void TIMER_attach_tick_func(void (*func)()) { tick_func = func; }

void TIMER_detach_tick_func() { tick_func = NULL; }

/**
 * @brief ISR definitions for timer tick implementations
 *
 */
#if defined(TIMER_TICK_N) && TIMER_TICK_N == 0
void TIMER_tick_init(uint8_t clockSelect)
{
    // TCCR0A default - use normal mode with top of 0xff and no ocr outputs

    TCCR0B = clockSelect; // setup the clock source and prescaler
    TIMSK0 = _BV(TOIE0);
}

ISR(TIMER0_OVF_vect)
{
    current_ticks++;
    if (tick_func)
    {
        tick_func();
    }
}
#elif defined(TIMER_TICK_N) && TIMER_TICK_N == 1
void TIMER_tick_init(uint8_t clockSelect)
{
    // TCCR1A default - use normal mode with top of 0xffff and no ocr outputs
    TCCR1B = clockSelect; // setup the clock source and prescaler
    TIMSK1 = _BV(TOIE1);
}

ISR(TIMER1_OVF_vect)
{
    current_ticks++;
    if (tick_func)
    {
        tick_func();
    }
}
#elif defined(TIMER_TICK_N) && TIMER_TICK_N == 2
void TIMER_tick_init(uint8_t clockSelect)
{
    // TCCR1A default - use normal mode with top of 0xff and no ocr outputs
    TCCR2B = clockSelect; // setup the clock source and prescaler
    TIMSK2 = _BV(TOIE2);
}

ISR(TIMER2_OVF_vect)
{
    current_ticks++;
    if (tick_func)
    {
        tick_func();
    }
}
#endif

#endif