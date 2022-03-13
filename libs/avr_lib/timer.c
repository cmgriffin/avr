#include "timer.h"
#include "debug.h"
#include "global.h"
#include <avr/interrupt.h>
#include <util/atomic.h>

void TIMER0_init(Timer_Init_Typedef *init)
{
    // configure the compare outputs per init and use fast PWM mode with top of 0xff
    TCCR0A = init->ocConfig | _BV(WGM01) | _BV(WGM00);
    // setup the clock source and prescaler
    TCCR0B = init->clockSelect;
    if (init->overflowInteruptEn)
    {
        TIMSK0 = _BV(TOIE0);
    }
}

// continuously updated to track current number of ticks since rollover
// needs to be volatile since it is only updated in the ISR
static volatile uint16_t current_ticks;

static void (*tick_func)();

uint16_t TIMER_getTicks()
{
    uint16_t copy;
    ATOMIC_BLOCK(ATOMIC_FORCEON)
    {
        copy = current_ticks;
    }
    return copy;
}

void TIMER_delayTicks(uint16_t ticks)
{
    uint16_t start;
    start = TIMER_getTicks();

    do
    {
    } while (TIMER_getTicks() < start + ticks);
}

void TIMER_delay_ms(uint16_t ms)
{
    uint16_t ticks = (uint16_t)(((uint32_t)ms * (F_CPU / 1000UL) + (TIMER_TICK_CLK_DIV >> 1)) / TIMER_TICK_CLK_DIV);
    _DEBUG("ms=%d, ticks=%d", ms, ticks);
    TIMER_delayTicks(ticks);
}

uint16_t TIMER_time_ms()
{
    uint32_t ticks = (uint32_t)TIMER_getTicks();
    _DEBUG("current ticks=%d", ticks);
    return (uint16_t)((TIMER_TICK_CLK_DIV * ticks + (F_CPU / 2000)) / (F_CPU / 1000));
}

void TIMER_attach_tick_func(void (*func)())
{
    tick_func = func;
}

void TIMER_detach_tick_func()
{
    tick_func = NULL;
}

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
