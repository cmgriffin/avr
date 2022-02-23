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

void UART_TransmitByte(uint8_t c);

uint8_t UART_ReceiveByte();

int UART_putChar(char c, FILE *stream);

// Receive a single char (byte) from the UART
int UART_getChar(FILE *stream);

// Discard data from UART until none remains
void UART_flush(void);

// A blocking read function that reads chars into *s until linefeed
// Ignores chars once the limit is reached
void UART_readString(char *s, uint8_t maxLength, uint8_t echo);

#endif // UART_H
