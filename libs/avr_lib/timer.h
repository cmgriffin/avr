/**
 * @file timer.h
 * @author C. Griffin
 * @brief A module of functions and macros relating to the AVR timer peripherals
 * @version 0.1
 * @date 2022-03-13
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __TIMER__
#define __TIMER__

#include "global.h"
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint8_t clockSelect : 3;     // configure the clock and prescaller for the timer
    uint8_t comConfig : 4;       // configure the output compare pin behavior
    uint8_t wgmConfig : 4;       // configure the output compare pin behavior
    bool overflowInteruptEn : 1; // enable the overflow interupt
    bool matchAInteruptEn : 1;   // enable the match A interupt
    bool matchBInteruptEn : 1;   // enable the match B interupt
} Timer_Init_Typedef;

#ifdef TCCR0A
#define TIMER0_CLK_OFF 0x0
#define TIMER0_CLK_DIV1 _BV(CS00)
#define TIMER0_CLK_DIV8 _BV(CS01)
#define TIMER0_CLK_DIV64 (_BV(CS00) | _BV(CS01))
#define TIMER0_CLK_DIV256 _BV(CS02)
#define TIMER0_CLK_DIV1024 (_BV(CS00) | _BV(CS02))
#define TIMER0_CLK_T_FALL (_BV(CS01) | _BV(CS02))
#define TIMER0_CLK_T_RISE (_BV(CS00) | _BV(CS01) | _BV(CS02))

#define TIMER0_WGM_NORMAL 0x0
#define TIMER0_WGM_CTC_OCRA _BV(WGM01)
#define TIMER0_WGM_PWM_PHA_FF _BV(WGM00)
#define TIMER0_WGM_PWM_FAST_FF (_BV(WGM00) | _BV(WGM01))
#define TIMER0_WGM_PWM_PHA_OCRA (_BV(WGM00) | _BV(WGM02))
#define TIMER0_WGM_PWM_FAST_FF (_BV(WGM00) | _BV(WGM01) | _BV(WGM02))

#define TIMER_OCA_OFF 0x0
#define TIMER_OCA_TOGGLE _BV(COM0A0)
#define TIMER_OCA_CLEAR _BV(COM0A1)
#define TIMER_OCA_SET (_BV(COM0A0) | _BV(COM0A1))

#define TIMER_OCB_OFF 0x0
#define TIMER_OCB_TOGGLE _BV(COM0B0)
#define TIMER_OCB_CLEAR _BV(COM0B1)
#define TIMER_OCB_SET (_BV(COM0B0) | _BV(COM0B1))

void TIMER0_init(Timer_Init_Typedef *init, bool clearFirst);

#endif // TCCR0A

#ifdef TCCR1A
#define TIMER1_CLK_OFF 0x0
#define TIMER1_CLK_DIV1 _BV(CS00)
#define TIMER1_CLK_DIV8 _BV(CS01)
#define TIMER1_CLK_DIV64 (_BV(CS00) | _BV(CS01))
#define TIMER1_CLK_DIV256 _BV(CS02)
#define TIMER1_CLK_DIV1024 (_BV(CS00) | _BV(CS02))
#define TIMER1_CLK_T_FALL (_BV(CS01) | _BV(CS02))
#define TIMER1_CLK_T_RISE (_BV(CS00) | _BV(CS01) | _BV(CS02))

#define TIMER1_WGM_NORMAL 0x0
#define TIMER1_WGM_PWM_PHA_FF _BV(WGM10)
#define TIMER1_WGM_PWM_PHA_1FF _BV(WGM11)
#define TIMER1_WGM_PWM_PHA_3FF (_BV(WGM10) | _BV(WGM11))
#define TIMER1_WGM_CTC_OCRA _BV(WGM12)
#define TIMER1_WGM_PWM_FAST_FF (_BV(WGM10) | _BV(WGM12))
#define TIMER1_WGM_PWM_FAST_1FF (_BV(WGM11) | _BV(WGM12))
#define TIMER1_WGM_PWM_FAST_3FF (_BV(WGM10) | _BV(WGM11) | _BV(WGM12))
#define TIMER1_WGM_PWM_PHAFREQ_ICR _BV(WGM13)
#define TIMER1_WGM_PWM_PHAFREQ_OCRA (_BV(WGM10) | _BV(WGM13))
#define TIMER1_WGM_PWM_PHA_ICR (_BV(WGM11) | _BV(WGM13))
#define TIMER1_WGM_PWM_PHA_OCRA (_BV(WGM10) | _BV(WGM11) | _BV(WGM13))
#define TIMER1_WGM_CTC_ICR (_BV(WGM12) | _BV(WGM13))
#define TIMER1_WGM_PWM_FAST_ICR (_BV(WGM11) | _BV(WGM12) | _BV(WGM13))
#define TIMER1_WGM_PWM_FAST_OCRA (_BV(WGM10) | _BV(WGM11) | _BV(WGM12) | _BV(WGM13))

void TIMER1_init(Timer_Init_Typedef *init, bool clearFirst);

#endif // TCCR1A

#ifdef TCCR2A
#define TIMER2_CLK_OFF 0x0
#define TIMER2_CLK_DIV1 _BV(CS00)
#define TIMER2_CLK_DIV8 _BV(CS01)
#define TIMER2_CLK_DIV32 (_BV(CS00) | _BV(CS01))
#define TIMER2_CLK_DIV64 _BV(CS02)
#define TIMER2_CLK_DIV128 (_BV(CS00) | _BV(CS02))
#define TIMER2_CLK_DIV256 (_BV(CS01) | _BV(CS02))
#define TIMER2_CLK_DIV1024 (_BV(CS00) | _BV(CS01) | _BV(CS02))

#define TIMER2_WGM_NORMAL 0x0
#define TIMER2_WGM_PWM_PHA_FF _BV(WGM20)
#define TIMER2_WGM_CTC_OCRA _BV(WGM21)
#define TIMER2_WGM_PWM_FAST_FF (_BV(WGM20) | _BV(WGM21))
#define TIMER2_WGM_PWM_PHA_OCRA (_BV(WGM20) | _BV(WGM22))
#define TIMER2_WGM_PWM_FAST_FF (_BV(WGM20) | _BV(WGM21) | _BV(WGM22))

void TIMER2_init(Timer_Init_Typedef *init, bool clearFirst);
#endif // TCCR2A

#if TIMER_TICK_N == 0 || TIMER_TICK_N == 2
#define TIMER_TICK_CLK_DIV (TIMER_TICK_PRESCALLER * 256UL)
#else
#define TIMER_TICK_CLK_DIV (TIMER_TICK_PRESCALLER * 65536UL)
#endif

#ifdef TIMER_TICK_N

#define __TIMER_TICK_MAX_MS_DELAY (0xffff * TIMER_TICK_CLK_DIV / (F_CPU / 1000))
#if __TIMER_TICK_MAX_MS_DELAY < 0xffff
#pragma message "TIMER_delay_ms MAX DELAY IS LESS THAN MAX_UNINT16"
#pragma message "TIMER_delay_ms MAX DELAY = " STRING(__TIMER_TICK_MAX_MS_DELAY)
#endif

/**
 * @brief A simplified and compact initialization function for the tick functions
 * Sets up the timer defined by TIMER_TICK_N to generate TOV interupts
 *
 * @param clockSelect value to set for CS0-3 of the targeted timer
 */
void TIMER_tick_init(uint8_t clockSelect);

/**
 * @brief Returns the current tick counter value
 *
 * @return uint16_t current tick counter value
 */
uint16_t TIMER_getTicks();

/**
 * @brief Delay execution by a number of ticks
 *
 * @param ticks number of ticks to delay by
 */
void TIMER_delayTicks(uint16_t ticks);

/**
 * @brief Delay execution by a number of ms
 *
 * @param ms ms to delay by
 */
void TIMER_delay_ms(uint16_t ms);

/**
 * @brief Attach a function to be called by the tick timer TOV ISR
 * This is executed in addition the update of the tick counter
 *
 * @param func pointer to a void function with no arguments
 */
void TIMER_attach_tick_func(void (*func)());

/**
 * @brief Detach the function from the tick timer TOV ISR
 *
 */
void TIMER_detach_tick_func();

/**
 * @brief Return the current tick counter value converted to ms
 *
 */
uint16_t TIMER_time_ms();

#endif // TIMER_TICK_N

#endif // __TIMER__