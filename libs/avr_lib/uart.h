/*
 * Provides commonly used functions for dealing with the UART
 *
 */
#ifndef UART_H
#define UART_H

#include "global.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Initialize the UART peripheral
void UART_init(void);

bool UART_TransmitByte(uint8_t c, bool blocking);

bool UART_ReceiveByte(uint8_t *c, bool blocking);

int UART_putChar(char c, FILE *stream);

// Receive a single char (byte) from the UART
int UART_getChar(FILE *stream);

// Discard data from UART until none remains
void UART_flush(void);

char *UART_readLine(char *s, uint8_t n, char **rxptr);

void UART_printStr(const char *s);

void UART_printStr_p(const char *s);

void UART_print_u16(uint16_t v, uint8_t fp, bool rj, bool neg_sign);

void UART_print_i16(uint16_t v, uint8_t fp, bool rj);

#endif // UART_H
