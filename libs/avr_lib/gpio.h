#ifndef GPIO_H
#define GPIO_H

#include "global.h"
#include <avr/io.h>

/**
 * @brief GPIO_PORT_t defines the physical layout of the AVR hardware IO registers
 *
 */
typedef struct
{
    uint8_t pin;
    uint8_t ddr;
    uint8_t port;
} GPIO_PORT_t;

/**
 * @brief GPIO_TypeDef is a struct that holds all necessary references necessary to manipulate AVR GPIO
 * *port - a pointer to a struct of the IOs related to AVR GPIO
 * mask - bit mask defining pins to be modified by GPIO functions
 */
typedef struct
{
    GPIO_PORT_t *port;
    uint8_t mask;
} GPIO_TypeDef;

/**
 * @brief macro to simplify defining of basic GPIO_Typedefs
 *
 */
#define _PIN_STRUCT(port, pin_mask) \
    {                               \
        port, pin_mask              \
    }

#define GPIO_PORTB (GPIO_PORT_t *)&PINB
#define GPIO_PORTC (GPIO_PORT_t *)&PINC
#define GPIO_PORTD (GPIO_PORT_t *)&PIND

#define GPIO_PB0 _PIN_STRUCT(GPIO_PORTB, _BV(PB0))
#define GPIO_PB1 _PIN_STRUCT(GPIO_PORTB, _BV(PB1))
#define GPIO_PB2 _PIN_STRUCT(GPIO_PORTB, _BV(PB2))
#define GPIO_PB3 _PIN_STRUCT(GPIO_PORTB, _BV(PB3))
#define GPIO_PB4 _PIN_STRUCT(GPIO_PORTB, _BV(PB4))
#define GPIO_PB5 _PIN_STRUCT(GPIO_PORTB, _BV(PB5))
#define GPIO_PB6 _PIN_STRUCT(GPIO_PORTB, _BV(PB6))
#define GPIO_PB7 _PIN_STRUCT(GPIO_PORTB, _BV(PB7))

#define GPIO_PC0 _PIN_STRUCT(GPIO_PORTC, _BV(PC0))
#define GPIO_PC1 _PIN_STRUCT(GPIO_PORTC, _BV(PC1))
#define GPIO_PC2 _PIN_STRUCT(GPIO_PORTC, _BV(PC2))
#define GPIO_PC3 _PIN_STRUCT(GPIO_PORTC, _BV(PC3))
#define GPIO_PC4 _PIN_STRUCT(GPIO_PORTC, _BV(PC4))
#define GPIO_PC5 _PIN_STRUCT(GPIO_PORTC, _BV(PC5))
#define GPIO_PC6 _PIN_STRUCT(GPIO_PORTC, _BV(PC6))

#define GPIO_PD0 _PIN_STRUCT(GPIO_PORTD, _BV(PD0))
#define GPIO_PD1 _PIN_STRUCT(GPIO_PORTD, _BV(PD1))
#define GPIO_PD2 _PIN_STRUCT(GPIO_PORTD, _BV(PD2))
#define GPIO_PD3 _PIN_STRUCT(GPIO_PORTD, _BV(PD3))
#define GPIO_PD4 _PIN_STRUCT(GPIO_PORTD, _BV(PD4))
#define GPIO_PD5 _PIN_STRUCT(GPIO_PORTD, _BV(PD5))
#define GPIO_PD6 _PIN_STRUCT(GPIO_PORTD, _BV(PD6))
#define GPIO_PD7 _PIN_STRUCT(GPIO_PORTD, _BV(PD7))

// Typical Arduino defines for Atmega328, 168, etc
#define D0 GPIO_PD0
#define D1 GPIO_PD1
#define D2 GPIO_PD2
#define D3 GPIO_PD3
#define D4 GPIO_PD4
#define D5 GPIO_PD5
#define D6 GPIO_PD6
#define D7 GPIO_PD7
#define D8 GPIO_PB0
#define D9 GPIO_PB1
#define D10 GPIO_PB2
#define D11 GPIO_PB3
#define D12 GPIO_PB4
#define D13 GPIO_PB5
#define A0 GPIO_PC0
#define A1 GPIO_PC1
#define A2 GPIO_PC2
#define A3 GPIO_PC3
#define A4 GPIO_PC4
#define A5 GPIO_PC5

// More granular defines for setting up structs operating on multiple pins
#define PORT_D0_7 GPIO_PORTD
#define D0_MASK _BV(PD0)
#define D1_MASK _BV(PD1)
#define D2_MASK _BV(PD2)
#define D3_MASK _BV(PD3)
#define D4_MASK _BV(PD4)
#define D5_MASK _BV(PD5)
#define D6_MASK _BV(PD6)
#define D7_MASK _BV(PD7)

#define PORT_D8_13 GPIO_PORTB
#define D8_MASK _BV(PB0)
#define D9_MASK _BV(PB1)
#define D10_MASK _BV(PB2)
#define D11_MASK _BV(PB3)
#define D12_MASK _BV(PB4)
#define D13_MASK _BV(PB5)

#define PORT_A0_5 GPIO_PORTC
#define A0_MASK _BV(PC0)
#define A1_MASK _BV(PC1)
#define A2_MASK _BV(PC2)
#define A3_MASK _BV(PC3)
#define A4_MASK _BV(PC4)
#define A5_MASK _BV(PC5)

/*
GPIO struct based pin functions. Similar implementation to Pololu's FastIO lib for Arduino.
For these to be most efficient values must be known at compile time. However, this is not
required.
*/

// Sets pin(s) in struct to input
__attribute__((always_inline)) static inline void GPIO_setInput(const GPIO_TypeDef *gpio)
{
    gpio->port->ddr &= ~(gpio->mask);
}

// Sets pin(s) in struct to output
__attribute__((always_inline)) static inline void GPIO_setOutput(const GPIO_TypeDef *gpio)
{
    gpio->port->ddr |= gpio->mask;
}

// Sets pin(s) direction to value provided. Will both set and clear bits. Value must line up pin_mask that is defined in struct.
__attribute__((always_inline)) static inline void GPIO_setDir(const GPIO_TypeDef *gpio, uint8_t dir)
{
    gpio->port->ddr = (gpio->port->ddr & ~gpio->mask) | dir;
}

// Sets all pins interpreting value as a logical value
__attribute__((always_inline)) static inline void GPIO_setDirLogical(const GPIO_TypeDef *gpio, uint8_t dir)
{
    if (dir)
    {
        gpio->port->ddr |= gpio->mask;
    }
    else
    {
        gpio->port->ddr &= gpio->mask;
    }
}

// Toggles the pin(s) value. Uses PINx=1 toggle function which should be faster than PORT^=1
__attribute__((always_inline)) static inline void GPIO_toggleValue(const GPIO_TypeDef *gpio)
{
    gpio->port->pin |= gpio->mask;
}

// Sets the pin value high
__attribute__((always_inline)) static inline void GPIO_setValueHigh(const GPIO_TypeDef *gpio)
{
    gpio->port->port |= gpio->mask;
}

// Set the pin value to low
__attribute__((always_inline)) static inline void GPIO_setValueLow(const GPIO_TypeDef *gpio)
{
    gpio->port->port &= ~(gpio->mask);
}

// Sets pin(s) to value provided. Will both set and clear bits. Value must line up pin_mask that is defined in struct.
__attribute__((always_inline)) static inline void GPIO_setValue(const GPIO_TypeDef *gpio, uint8_t value)
{
    gpio->port->port = (gpio->port->port & ~gpio->mask) | value;
}

// Sets pin(s) interpreting value as a logical value
__attribute__((always_inline)) static inline void GPIO_setValueLogical(const GPIO_TypeDef *gpio, uint8_t value)
{
    if (value)
    {
        gpio->port->port |= gpio->mask;
    }
    else
    {
        gpio->port->port &= gpio->mask;
    }
}

// Returns the pin(s) input state. Will be logically true if any pin is high.
__attribute__((always_inline)) static inline uint8_t GPIO_getInput(const GPIO_TypeDef *gpio)
{
    return gpio->port->port & gpio->mask;
}

#endif /* GPIO_H */
