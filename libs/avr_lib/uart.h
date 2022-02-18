/*
 * Provides commonly used functions for dealing with the UART
 *
 */
#ifndef UART_H
#define UART_H

#include "global.h"
#include <stdint.h>
#include <stdio.h>

// Initialize the UART peripheral
void UART_init(void);

int UART_putChar(char c, FILE *stream);

// Send a single char (byte) from the UART
void UART_sendChar(char c);

int UART_putChar(char c, FILE *stream);

// Receive a single char (byte) from the UART
char UART_getChar(void);

// Discard data from UART until none remains
void UART_flush(void);

// A blocking read function that reads chars into *s until linefeed
// Ignores chars once the limit is reached
void UART_readString(char *s, uint8_t maxLength, uint8_t echo);

#ifdef UART_RX_INTERUPT

uint8_t UART_available()
{
    return rxbuff.num_entries;
}

uint8_t UART_getBufChar()
{
    return BUFFER_num_entries(&rxbuff);
}

#endif

#endif // UART_H
