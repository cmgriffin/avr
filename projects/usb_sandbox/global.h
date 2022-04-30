#ifndef __GLOBAL__
#define __GLOBAL__

#define STRING2(x) #x
#define STRING(x)  STRING2(x)

// Global Defines (used by many avr-libc libaries)

#define F_CPU 16000000UL

// Debugging
//#define DEBUG // enable debugging globally

// UART
//#define UART_DEBUG
#define BAUD 9600
// #define UART_RX_INTERUPT       // enable interupt driven UART recieving
// #define UART_RX_BUFFER_SIZE 10 // recieve buffer size when UART is interupt
// driven #define UART_TX_INTERUPT       // enable interupt driven UART
// transmittions #define UART_TX_BUFFER_SIZE 10 // transmit buffer size when
// UART is interupt driven

// #define UART_GETCHAR_BUFFER_SIZE 10

//#define UART_INIT_STDOUT
// #define UART_INIT_STDIN

// TIMER
//#define TIMER_DEBUG
//#define TIMER_TICK_N 0
#define TIMER_TICK_PRESCALLER 64UL

// ADC
//#define ADC_DEBUG
//#define ADC_USE_SLEEP_MODE
#define ADC_REF_SETTLE_TIME 1 // ms

// I2C
//#define I2C_DEBUG

// Arduino Hardware
#define LED_PIN PB5 // typically the builtin LED is connected to PB5

#define __ASSERT_USE_STDERR

#endif