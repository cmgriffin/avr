#include "uart.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <debug.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <util/setbaud.h>

#include "buffer.h"
#include "global.h"
#include "stream.h"

#ifndef UART_N

#if UART_N == 0
#define UBRRnH UBRR0H
#define UBRRnL UBRR0L
#define UCSRnA UCSR0A
#define U2Xn U2X0
#define UDREn UDRE0
#define RXCn RXC0

#define UCSRnB UCSR0B
#define TXENn TXEN0
#define RXENn RXEN0
#define TXCIEn TXCIE0
#define RXCIEn RXCIE0

#define UCSRnC UCSR0C
#define UCSZn1 UCSZ01
#define UCSZn0 UCSZ00

#define UDRn UDR0

#elif UART_N == 1
#define UBRRnH UBRR1H
#define UBRRnL UBRR1L
#define UCSRnA UCSR1A
#define U2Xn U2X1
#define UDREn UDRE1
#define RXCn RXC1

#define UCSRnB UCSR1B
#define TXENn TXEN1
#define RXENn RXEN1
#define TXCIEn TXCIE1
#define RXCIEn RXCIE1

#define UCSRnC UCSR1C
#define UCSZn1 UCSZ11
#define UCSZn0 UCSZ10

#define UDRn UDR1

#endif

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
static FILE uart_stream =
    FDEV_SETUP_STREAM(UART_putChar, NULL, _FDEV_SETUP_WRITE);
#elif !defined(UART_INIT_STDOUT) && defined(UART_INIT_STDIN)
static FILE uart_stream =
    FDEV_SETUP_STREAM(NULL, UART_getChar, _FDEV_SETUP_READ);
#elif defined(UART_INIT_STDOUT) && defined(UART_INIT_STDIN)
static FILE uart_stream =
    FDEV_SETUP_STREAM(UART_putChar, UART_getChar, _FDEV_SETUP_RW);
#endif

static stream_t uart_io = STREAM_CREATE(UART_TransmitByte, UART_ReceiveByte);

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
bool UART_TransmitByte(uint8_t c, bool blocking)
{
#ifdef UART_TX_INTERUPT

  if (!tx_busy)
  {
    // send the first byte to start the ISR
    tx_busy = true;
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c; /* send data */
    return true;
  }
  else
  {
    // ISR will handle sending subsequent bytes
    // in case the buffer is full we need to wait
    while (BUFFER_full(&txbuff) && blocking)
    {
      ;
    }
    return BUFFER_enqueue(&txbuff, c);
  }
#else // not interupt driven
  if (blocking)
  {
    /* Wait for empty transmit buffer */
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c; /* send data */
    return true;
  }
  else if (bit_is_clear(UCSR0A, UDRE0))
  { // buffer full and not waiting
    return false;
  }
  else
  {
    // buffer empty
    UDR0 = c;
    return true;
  }
#endif
}

/**
 * @brief Low level routine for receiving a byte of data through the UART
 * Method utilized is dependant on if interupt driven transfers are used.
 *
 * @return uint8_t
 */
bool UART_ReceiveByte(uint8_t *c, bool blocking)
{
  // TODO: could pack these into two conditions...
#ifdef UART_RX_INTERUPT
  if (blocking)
  {
    while (BUFFER_empty(&rxbuff))
    {
      ;
    }
    *c = BUFFER_dequeue(&rxbuff);
    return true;
  }
  else if (BUFFER_empty(&rxbuff))
  {
    return false;
  }
  else
  {
    *c = BUFFER_dequeue(&rxbuff);
    return true;
  }

#else
  if (blocking)
  {
    loop_until_bit_is_set(UCSR0A, RXC0); /* Wait for incoming data */
    *c = UDR0;                           /* return register value */
    return true;
  }
  else if (bit_is_clear(UCSR0A, RXC0))
  {
    return false;
  }
  else
  {
    *c = UDR0;
    return true;
  }

#endif
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
  return STREAM_putChar(c, &uart_io, stream);
}

/**
 * @brief Get a character from the UART
 * Supports some basic editing
 *
 * @param stream
 * @return int
 */
#ifdef UART_GETCHAR_BUFFER_SIZE
int UART_getChar(FILE *stream)
{
  static char buff[UART_GETCHAR_BUFFER_SIZE];
  static char *rxptr; // ptr to the current position in the buffer when reading

  return STREAM_getChar(buff, UART_GETCHAR_BUFFER_SIZE, &rxptr, &uart_io,
                        stream);
}
#endif

char *UART_readLine(char *s, uint8_t n, char **rxptr)
{
  return STREAM_readLine(s, n, rxptr, &uart_io);
}

void UART_printStr(const char *s) { STREAM_printStr(s, &uart_io); }

void UART_printStr_p(const char *s) { STREAM_printStr_p(s, &uart_io); }

void UART_print_u16(uint16_t v, uint8_t fp, bool rj, bool neg_sign)
{
  STREAM_print_u16(v, fp, rj, neg_sign, &uart_io);
}

void UART_print_i16(uint16_t v, uint8_t fp, bool rj)
{
  STREAM_print_i16(v, fp, rj, &uart_io);
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

#endif
