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
#include "timer_conf.h"
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
    uint8_t clockSelect;
    uint8_t ocConfig;
    uint8_t wgmConfig;
    uint8_t interuptEnable;
} Timer_Init_Typedef;

#ifdef TCCR0A
#define TIMER0_CLK_OFF           0x0
#define TIMER0_CLK_DIV1          0x1
#define TIMER0_CLK_DIV8          0x2
#define TIMER0_CLK_DIV64         0x3
#define TIMER0_CLK_DIV256        0x4
#define TIMER0_CLK_DIV1024       0x5
#define TIMER0_CLK_T_FALL        0x6
#define TIMER0_CLK_T_RISE        0x7

#define TIMER0_WGM_NORMAL        0x0
#define TIMER0_WGM_PWM_PHA_FF    0x1
#define TIMER0_WGM_CTC_OCRA      0x2
#define TIMER0_WGM_PWM_FAST_FF   0x3
#define TIMER0_WGM_PWM_PHA_OCRA  0x5
#define TIMER0_WGM_PWM_FAST_OCRA 0x7

#define TIMER_OCA_OFF            0x0
#define TIMER_OCA_TOGGLE         0x1
#define TIMER_OCA_CLEAR          0x2
#define TIMER_OCA_SET            0x3

#define TIMER_OCB_OFF            0x0
#define TIMER_OCB_TOGGLE         0x4
#define TIMER_OCB_CLEAR          0x8
#define TIMER_OCB_SET            0xC

#define TIMER_INTERUPT_NONE      0
#define TIMER_INTERUPT_TOV       _BV(TOIE0)
#define TIMER_INTERUPT_OCA       _BV(OCIE0A)
#define TIMER_INTERUPT_OCB       _BV(OCIE0B)

// some alias macros for ease of use
#define TIMER0_OCA_PD6 OCR0A
#define TIMER0_OCB_PD5 OCR0B
#define TIMER0_OCA_D6  OCR0A
#define TIMER0_OCB_D5  OCR0B

void TIMER0_init(const Timer_Init_Typedef *init, bool clearFirst);

#endif // TCCR0A

#ifdef TCCR1A
#define TIMER1_CLK_OFF              0x0
#define TIMER1_CLK_DIV1             0x1
#define TIMER1_CLK_DIV8             0x2
#define TIMER1_CLK_DIV64            0x3
#define TIMER1_CLK_DIV256           0x4
#define TIMER1_CLK_DIV1024          0x5
#define TIMER1_CLK_T_FALL           0x6
#define TIMER1_CLK_T_RISE           0x7

#define TIMER1_WGM_NORMAL           0x0
#define TIMER1_WGM_PWM_PHA_FF       0x1
#define TIMER1_WGM_PWM_PHA_1FF      0x2
#define TIMER1_WGM_PWM_PHA_3FF      0x3
#define TIMER1_WGM_CTC_OCRA         0x4
#define TIMER1_WGM_PWM_FAST_FF      0x5
#define TIMER1_WGM_PWM_FAST_1FF     0x6
#define TIMER1_WGM_PWM_FAST_3FF     0x7
#define TIMER1_WGM_PWM_PHAFREQ_ICR  0x8
#define TIMER1_WGM_PWM_PHAFREQ_OCRA 0x9
#define TIMER1_WGM_PWM_PHA_ICR      0xA
#define TIMER1_WGM_PWM_PHA_OCRA     0xB
#define TIMER1_WGM_CTC_ICR          0xC
#define TIMER1_WGM_PWM_FAST_ICR     0xE
#define TIMER1_WGM_PWM_FAST_OCRA    0xF

// some alias macros for ease of use
#define TIMER1_OCA_PB1 OCR1A
#define TIMER1_OCB_PB2 OCR1B
#define TIMER1_OCA_D9  OCR1A
#define TIMER1_OCB_D10 OCR1B

void TIMER1_init(const Timer_Init_Typedef *init, bool clearFirst);

#endif // TCCR1A

#ifdef TCCR2A
#define TIMER2_CLK_OFF           0x0
#define TIMER2_CLK_DIV1          0x1
#define TIMER2_CLK_DIV8          0x2
#define TIMER2_CLK_DIV32         0x3
#define TIMER2_CLK_DIV64         0x4
#define TIMER2_CLK_DIV128        0x5
#define TIMER2_CLK_DIV256        0x6
#define TIMER2_CLK_DIV1024       0x7

#define TIMER2_WGM_NORMAL        0x0
#define TIMER2_WGM_PWM_PHA_FF    0x1
#define TIMER2_WGM_CTC_OCRA      0x2
#define TIMER2_WGM_PWM_FAST_FF   0x3
#define TIMER2_WGM_PWM_PHA_OCRA  0x5
#define TIMER2_WGM_PWM_FAST_OCRA 0x7

// some alias macros for ease of use
#define TIMER2_OCA_PB3 OCR2A
#define TIMER2_OCB_PD3 OCR2B
#define TIMER2_OCA_D11 OCR2A
#define TIMER2_OCB_D3  OCR2B

void TIMER2_init(const Timer_Init_Typedef *init, bool clearFirst);
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
 * @brief A simplified and compact initialization function for the tick
 * functions Sets up the timer defined by TIMER_TICK_N to generate TOV interupts
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