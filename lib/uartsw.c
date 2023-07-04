/*! \file uartsw.c \brief Software Interrupt-driven UART Driver. */
//*****************************************************************************
//
// File Name	: 'uartsw.c'
// Title		: Software Interrupt-driven UART Driver
// Author		: Pascal Stang - Copyright (C) 2002-2004
// Created		: 7/20/2002
// Revised		: 4/27/2004
// Version		: 0.1
// Target MCU	: Atmel AVR Series (intended for the ATmega16 and ATmega32)
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#if __has_include("uartsw_conf.h")
#include "uartsw_conf.h"

#include <avr/interrupt.h>
#include <buffer.h>
#include <gpio.h>
#include <stream.h>
#include <timer.h>

#include "global.h"
#include "timer.h"
#include "uartsw.h"

// Program ROM constants

// Global variables

#define BAUDRATE_DIV \
    (uint16_t)((F_CPU + (UARTSW_BAUD / 2UL)) / (UARTSW_BAUD * 1UL))

// uartsw transmit status and data variables
static volatile uint8_t TxBusy;
static volatile uint8_t TxData;
static volatile uint8_t TxBitNum;

// uartsw receive status and data variables
static volatile uint8_t RxBusy;
static volatile uint8_t RxData;
static volatile uint8_t RxBitNum;
// receive buffer
static uint8_t _rxbuff[UARTSW_RX_BUFFER_SIZE]; ///< uartsw receive buffer
static buffer_t rxbuff = BUFFER_CREATE(UARTSW_RX_BUFFER_SIZE, _rxbuff);

// functions

//! enable and initialize the software uart
void UARTSW_init(void)
{
    // initialize the ports
    GPIO_setValueHigh(&(GPIO_TypeDef)UARTSW_RX_PIN); // input pullup
    GPIO_setValueHigh(&(GPIO_TypeDef)UARTSW_TX_PIN);
    GPIO_setOutput(&(GPIO_TypeDef)UARTSW_TX_PIN);
    GPIO_setInput(&(GPIO_TypeDef)UARTSW_RX_PIN);

    // trigger on falling edge

    // TIMER1_init(&(Timer_Init_Typedef){.clockSelect    = TIMER1_CLK_DIV1,
    //                                   .interuptEnable = false,
    //                                   .ocConfig = TIMER_OCA_OFF |
    //                                   TIMER_OCB_OFF, .wgmConfig =
    //                                   TIMER1_WGM_NORMAL},
    //             false);

    TCCR1B |= TIMER1_CLK_DIV1;

    // setup the transmitter
    TxBusy = false;
    // disable OC1A interrupt
    TIMSK1 &= ~_BV(OCIE1A);

    // setup the receiver
    RxBusy = false;
    // disable OC1B interrupt
    TIMSK1 &= ~_BV(OCIE1B);

    // trigger on falling edge
    TCCR1B &= ~_BV(ICES1);

    // enable ICP interrupt
    TIMSK1 |= _BV(ICIE1);

    // turn on interrupts
    sei();
}

//! turns off software UART
void UARTSW_off(void)
{
    // disable interrupts
    TIMSK1 &= ~(_BV(OCIE1A) | _BV(OCIE1B) | _BV(ICIE1));
}

// //! returns the receive buffer structure
// cBuffer *UARTSW_GetRxBuffer(void)
// {
//     // return rx buffer pointer
//     return &UARTSW_RxBuffer;
// }

bool UARTSW_TransmitByte(uint8_t c, bool blocking)
{
    if (TxBusy && !blocking)
        return false;
    // wait until uart is ready
    while (TxBusy)
        ;
    // set busy flag
    TxBusy = true;
    // save data
    TxData = c;
    // set number of bits (+1 for stop bit)
    TxBitNum = 9;
    GPIO_setValueLow(&(GPIO_TypeDef)UARTSW_TX_PIN);

    // schedule the next bit
    OCR1A = TCNT1 + BAUDRATE_DIV;
    // clear OC1B interrupt flag
    TIFR1 |= _BV(OCF1A);
    // enable OC1A interrupt
    TIMSK1 |= _BV(OCIE1A);
    return true;
}

//! gets a byte (if available) from the uart receive buffer
bool UARTSW_ReceiveByte(uint8_t *c, bool blocking)
{
    // make sure we have a receive buffer
    // make sure we have data
    if (!BUFFER_available(&rxbuff) && !blocking)
        return false;
    while (!BUFFER_available(&rxbuff))
    {
        ;
    }
    // get byte from beginning of buffer
    *c = BUFFER_dequeue(&rxbuff);
    return true;
}

ISR(TIMER1_COMPA_vect)
{
    if (TxBitNum)
    {
        // there are bits still waiting to be transmitted
        if (TxBitNum > 1)
        {
            // transmit data bits (inverted, LSB first)

            if ((TxData & 0x01))

                GPIO_setValueHigh(&(GPIO_TypeDef)UARTSW_TX_PIN);
            else
                GPIO_setValueLow(&(GPIO_TypeDef)UARTSW_TX_PIN);
            // shift bits down
            TxData = TxData >> 1;
        }
        else
        {
            // transmit stop bit

            GPIO_setValueHigh(&(GPIO_TypeDef)UARTSW_TX_PIN);
        }
        // schedule the next bit
        OCR1A = OCR1A + BAUDRATE_DIV;
        // count down
        TxBitNum--;
    }
    else
    {
        // transmission is done
        // disable OC1A interupt
        TIMSK1 &= ~_BV(OCIE1A);
        // clear busy flag
        TxBusy = false;
    }
}

ISR(TIMER1_CAPT_vect)
{
    // this function runs on either:
    // - a rising edge interrupt
    // - OC1B
    if (!RxBusy)
    {
        // this is a start bit
        // disable ICP interrupt
        TIMSK1 &= ~_BV(ICIE1);
        // schedule data bit sampling 1.5 bit periods from now
        OCR1B = TCNT1 + BAUDRATE_DIV + (BAUDRATE_DIV >> 1);

        // clear OC1B interrupt flag
        TIFR1 |= _BV(OCF1B);
        // enable OC1B interrupt
        TIMSK1 |= _BV(OCIE1B);
        // set start bit flag
        RxBusy = true;
        // reset bit counter
        RxBitNum = 0;
        // reset data
        RxData = 0;
    }
    else
    {
        // start bit has already been received
        // we're in the data bits

        // shift data byte to make room for new bit
        RxData = RxData >> 1;

        // sample the data line

        if (GPIO_getInput(&(GPIO_TypeDef)UARTSW_RX_PIN))

        {
            // serial line is marking
            // record '1' bit
            RxData |= 0x80;
        }

        // increment bit counter
        RxBitNum++;
        // schedule next bit sample
        OCR1B = OCR1B + BAUDRATE_DIV;

        // check if we have a full byte
        if (RxBitNum >= 8)
        {
            // save data in receive buffer
            BUFFER_enqueue(&rxbuff, RxData);
            // disable OC1B interrupt
            TIMSK1 &= ~_BV(OCIE1B);
            // clear ICP interrupt flag
            TIFR1 |= _BV(ICF1);
            // enable ICP interrupt
            TIMSK1 |= _BV(ICIE1);
            // clear start bit flag
            RxBusy = false;
        }
    }
}

ISR(TIMER1_COMPB_vect, ISR_ALIASOF(TIMER1_CAPT_vect));

/*
void uartswRxBitService(void)
{
        uint16_t thisBitTime;
        uint8_t bitperiods;
        uint8_t i;

        // bit transition was detected
        // record bit's edge time
        thisBitTime = inw(ICR1);

        cbi(PORTB, 0);

        if(!UartswRxStartBit)
        {
                // this is a start bit
                // switch to falling-edge trigger
                cbi(TCCR1B, ICES1);
                // record bit time
                UartswRxBitTime = thisBitTime;
                // set start bit flag
                UartswRxStartBit = true;
                // reset bit counter
                RxBitNum = 0;
                // reset data
                RxData = 0;
        }
        else
        {
                // start bit has already been received
                // we're in the data bits

                // how many bit periods since last edge?
                bitperiods = (thisBitTime - UartswRxBitTime +
UartswBaudRateDiv/2)/UartswBaudRateDiv;
                // set last edge time
                UartswRxBitTime = thisBitTime;

                if(bitperiods > 10)
                {
                        // switch to trigger on rising edge
                        sbi(TCCR1B, ICES1);
                        // clear start bit flag
                        UartswRxStartBit = false;
                }
                else
                {


                if( inb(TCCR1B) & (1<<ICES1) )
                {
                        // just triggered on a rising edge
                        // previous bits were zero
                        // shift in the data (data bits are inverted)
                        for(i=0; i<bitperiods; i++)
                        {
                                RxData = RxData<<1;
                                RxData |= 0x01;
                        }
                        // switch to trigger on falling edge
                        cbi(TCCR1B, ICES1);
                }
                else
                {
                        // just triggered on a falling edge
                        // previous bits were one
                        // shift in the data (data bits are inverted)
                        for(i=0; i<bitperiods; i++)
                        {
                                RxData = RxData<<1;
                        }
                        // switch to trigger on rising edge
                        sbi(TCCR1B, ICES1);
                }

                // increment bit counter
                RxBitNum += bitperiods;

                // check if we have a full byte + start bit
                if(bitperiods > 8)
                {
                        // save data in receive buffer
                        bufferAddToEnd(&uartswRxBuffer, RxData);
                        // switch to trigger on rising edge
                        sbi(TCCR1B, ICES1);
                        // clear start bit flag
                        UartswRxStartBit = false;
                }
                }
        }

        // turn off debug LEDs
        delay(10);
        sbi(PORTB, 0);
        sbi(PORTB, 1);
}
*/

#endif