#include "uart.h"
#include "buffer.h"
#include "global.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <debug.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <util/setbaud.h>

#ifdef UART_RX_INTERUPT
static uint8_t _rxbuff[UART_RX_BUFFER_SIZE];
static buffer_t rxbuff = BUFFER_CREATE(UART_RX_BUFFER_SIZE, _rxbuff);
#endif

#ifdef UART_TX_INTERUPT
static uint8_t _txbuff[UART_TX_BUFFER_SIZE];
static buffer_t txbuff = BUFFER_CREATE(UART_TX_BUFFER_SIZE, _txbuff);
volatile static bool tx_busy;
#endif

#if defined(UART_INIT_STDOUT) && !defined(UART_INIT_STDIN)
static FILE uart_stream = FDEV_SETUP_STREAM(UART_putChar, NULL, _FDEV_SETUP_WRITE);
#elif !defined(UART_INIT_STDOUT) && defined(UART_INIT_STDIN)
static FILE uart_stream = FDEV_SETUP_STREAM(NULL, UART_getChar, _FDEV_SETUP_READ);
#elif defined(UART_INIT_STDOUT) && defined(UART_INIT_STDIN)
static FILE uart_stream = FDEV_SETUP_STREAM(UART_putChar, UART_getChar, _FDEV_SETUP_RW);
#endif

/**
 * @brief Initialize the UART using baud defined and enabling the tx and rx
 * interupts as defined.
 * Also assign the stdout, stderr, and stdin as defined.
 *
 */
void UART_init(void)
{
	// set up the baud rate
	UBRR0H = UBRRH_VALUE; /* defined in setbaud.h */
	UBRR0L = UBRRL_VALUE;
#if USE_2X
	UCSR0A |= (1 << U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif

	/* Enable USART transmitter/receiver and receiver interupt*/
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);

#ifdef UART_RX_INTERUPT
	UCSR0B |= (1 << RXCIE0);
#endif
#ifdef UART_TX_INTERUPT
	UCSR0B |= (1 << TXCIE0);
#endif

	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8 data bits, 1 stop bit */

#ifdef UART_INIT_STDOUT
	stdout = &uart_stream;
#endif
#ifdef UART_INIT_STDIN
	stdin = &uart_stream;
#endif
}

/**
 * @brief Low level routine for sending a byte of data through the UART
 * Method utilized is dependant on if interupt driven transfers are used.
 *
 * @param c byte of data to send
 */
void UART_TransmitByte(uint8_t c)
{
#ifdef UART_TX_INTERUPT

	if (!tx_busy)
	{
		// send the first byte to start the ISR
		tx_busy = true;
		loop_until_bit_is_set(UCSR0A, UDRE0);
		UDR0 = c; /* send data */
	}
	else
	{
		// ISR will handle sending subsequent bytes
		// in case the buffer is full we need to wait
		while (BUFFER_full(&txbuff))
		{
			;
		}
		BUFFER_enqueue(&txbuff, c);
	}
#else // not interupt driven
	/* Wait for empty transmit buffer */
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;							 /* send data */
#endif
}

/**
 * @brief Low level routine for receiving a byte of data through the UART
 * Method utilized is dependant on if interupt driven transfers are used.
 *
 * @return uint8_t
 */
uint8_t UART_ReceiveByte()
{
	uint8_t c;
#ifdef UART_RX_INTERUPT
	while (BUFFER_empty(&rxbuff))
	{
		;
	}
	c = BUFFER_dequeue(&rxbuff);
#else
	loop_until_bit_is_set(UCSR0A, RXC0); /* Wait for incoming data */
	c = UDR0;							 /* return register value */
#endif
	return c;
}

/**
 * @brief Send a character down the UART
 *
 * @param c character to send
 * @param stream std stream structure
 * @return int
 */
int UART_putChar(char c, FILE *stream)
{
	if (c == '\n')
		UART_putChar('\r', stream);

	UART_TransmitByte((uint8_t)c);
	return 0;
}

/**
 * @brief Get a character from the UART
 * Supports some basic editing
 *
 * @param stream
 * @return int
 */
int UART_getChar(FILE *stream)
{
	char *cptr; // ptr to current position in the buffer when loading
	static char buff[UART_GETCHAR_BUFFER_SIZE];
	static char *rxptr; // ptr to the current position in the buffer when reading
	uint8_t c;

	if (!rxptr)
	{
		// nothing loaded up for the getChar yet

		for (cptr = buff; !rxptr;)
		{
			c = UART_ReceiveByte();
			switch (c)
			{
			case '\r':
				break;
				// c = '\n';
			case '\n':
				// line has been loaded set the read pointer
				*cptr = c;
				UART_putChar(c, stream);
				rxptr = buff;
				break;
			case '\b': // backspace
				if (cptr > buff)
				{
					// only backspace if buf is not empty
					UART_putChar('\b', stream); // move back one
					UART_putChar(' ', stream);	// clear the char
					UART_putChar('\b', stream); // move back
					cptr--;
				}
				break;
			default:
				if (!(cptr == buff + UART_GETCHAR_BUFFER_SIZE - 1))
				{
					*cptr++ = c;
					UART_putChar(c, stream);
				}
			}
		}
	}
	c = *rxptr++;
	if (c == '\n')
	{
		rxptr = 0;
	}
	return c;
}

void UART_flush(void)
{
	uint8_t dummy __attribute__((unused));
	while (UCSR0A & (1 << RXC0))
		dummy = UDR0;
}

#ifdef UART_RX_INTERUPT

ISR(USART_RX_vect)
{
	volatile uint8_t response;
	response = UDR0;
	if (bit_is_clear(UCSR0A, FE0)) // if no framing error occurred
	{
		BUFFER_enqueue(&rxbuff, response);
	}
}
#endif

#ifdef UART_TX_INTERUPT

ISR(USART_TX_vect)
{
	if (BUFFER_empty(&txbuff))
	{
		// buffer is empty, nothing to send
		tx_busy = false;
	}
	else
	{
		// send the next byte
		UDR0 = BUFFER_dequeue(&txbuff);
	}
}
#endif