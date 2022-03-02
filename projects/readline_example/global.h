#ifndef __GLOBAL__
#define __GLOBAL__

// Global Defines (used by many avr-libc libaries)

#define F_CPU 16000000L // if using a platformio board definition this would already be defined

// Debugging
#define DEBUG // enable debugging

// UART
#define BAUD 9600
// #define UART_RX_INTERUPT       // enable interupt driven UART recieving
// #define UART_RX_BUFFER_SIZE 10 // recieve buffer size when UART is interupt driven
// #define UART_TX_INTERUPT       // enable interupt driven UART transmittions
//#define UART_TX_BUFFER_SIZE 10 // transmit buffer size when UART is interupt driven

#define UART_GETCHAR_BUFFER_SIZE 10

//#define UART_INIT_STDOUT
//#define UART_INIT_STDIN

// Arduino Hardware
#define LED_PIN PB5 // typically the builtin LED is connected to PB5

#define __ASSERT_USE_STDERR

#endif