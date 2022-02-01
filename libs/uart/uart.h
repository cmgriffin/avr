/* 
* Provides commonly used functions for dealing with the UART
* 
*/
#ifndef UART_H
#define UART_H

#ifndef BAUD
#define BAUD 9600
#endif

#include <avr/pgmspace.h>
#include <stdint.h>

#include "print.h"

#define BUFFER_SIZE 20

extern char rxbuff[];
extern volatile uint8_t rx_rdy;

// Initialize the UART peripheral
void UART_init(void);

// Send a single char (byte) from the UART
void UART_sendChar(char c);

// Receive a single char (byte) from the UART
char UART_getChar(void);

// Discard data from UART until none remains
void UART_flush(void);

// A wrapper for printStr configured for the UART
static inline void UART_printStr(const char *s)
{
	printStr(UART_sendChar, s);
}

// A wrapper for printStr_p configured for the UART
static inline void UART_printStr_p(const char *s)
{
	printStr_p(UART_sendChar, s);
}

// A blocking read function that reads chars into *s until linefeed
// Ignores chars once the limit is reached
void UART_readString(char *s, uint8_t maxLength, uint8_t echo);

#endif //UART_H
