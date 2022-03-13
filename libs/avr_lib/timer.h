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

#define TIMER0_CLK_OFF 0x0
#define TIMER0_CLK_DIV1 0x1
#define TIMER0_CLK_DIV8 0x2
#define TIMER0_CLK_DIV64 0x3
#define TIMER0_CLK_DIV256 0x4
#define TIMER0_CLK_DIV1024 0x5
#define TIMER0_CLK_T_FALL 0x06
#define TIMER0_CLK_T_RISE 0x07

#define TIMER1_CLK_OFF 0x0
#define TIMER1_CLK_DIV1 0x1
#define TIMER1_CLK_DIV8 0x2
#define TIMER1_CLK_DIV64 0x3
#define TIMER1_CLK_DIV256 0x4
#define TIMER1_CLK_DIV1024 0x5
#define TIMER1_CLK_T_FALL 0x06
#define TIMER1_CLK_T_RISE 0x07

#define TIMER2_CLK_OFF 0x0
#define TIMER2_CLK_DIV1 0x1
#define TIMER2_CLK_DIV8 0x2
#define TIMER2_CLK_DIV32 0x3
#define TIMER2_CLK_DIV64 0x4
#define TIMER2_CLK_DIV128 0x5
#define TIMER2_CLK_DIV256 0x06
#define TIMER2_CLK_DIV1024 0x07

#define TIMER_OCA_OFF 0x0
#define TIMER_OCA_CLEAR 0x80
#define TIMER_OCA_SET 0xC0

#define TIMER_OCB_OFF 0x0
#define TIMER_OCB_CLEAR 0x20
#define TIMER_OCB_SET 0x30

#if TIMER_TICK_N == 0 || TIMER_TICK_N == 2
#define TIMER_TICK_CLK_DIV (TIMER_TICK_PRESCALLER * 256UL)
#else
#define TIMER_TICK_CLK_DIV (TIMER_TICK_PRESCALLER * 65536UL)
#endif

typedef struct
{
    uint8_t clockSelect : 3;     // configure the clock and prescaller for the timer
    uint8_t ocConfig : 4;        // configure the output compare pin behavior
    bool overflowInteruptEn : 1; // enable the overflow interupt
    bool matchAInteruptEn : 1;   // enable the match A interupt
    bool matchBInteruptEn : 1;   // enable the match B interupt
} Timer_Init_Typedef;

void TIMER0_init(Timer_Init_Typedef *init);

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

#endif

#endif // __TIMER__