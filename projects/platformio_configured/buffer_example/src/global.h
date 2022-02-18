#ifndef __GLOBAL__
#define __GLOBAL__

// Global Defines (used by many avr-libc libaries)
//#define F_CPU=16000000L // if using a platformio board definition this would already be defined

// Debugging
#define DEBUG // enable debugging

// UART
#define BAUD 9600
#define UART_RX_BUFFER_SIZE 20
//#define UART_RX_INTERUPT
#define UART_INIT_STDOUT
#define UART_INIT_STDERR

// Arduino Hardware
#define LED_PIN PB5 // typically the builtin LED is connected to PB5

#define __ASSERT_USE_STDERR

#endif