#ifndef GPIO_H
#define GPIO_H

#include <avr/io.h>

/*
GPIO_TypeDef is a struct that holds all necessary references necessary to manipulate AVR GPIO
*ppin - pointer to the PINx register
*pport - pointer to the PORTx register
*pddr - pointer to the DDRx register
pin_mask - bit mask defining pins to be modified by GPIO functions
*/
typedef struct
{
    volatile uint8_t *ppin;
    volatile uint8_t *pport;
    volatile uint8_t *pddr;
    uint8_t pin_mask;
} GPIO_TypeDef;

// macro to make predefining pins easier
#define _PIN_STRUCT(pin, port, ddr, pin_mask) \
    {                                         \
        &pin, &port, &ddr, pin_mask           \
    }

#define GPIO_PB0 _PIN_STRUCT(PINB, PORTB, DDRB, _BV(PB0))
#define GPIO_PB1 _PIN_STRUCT(PINB, PORTB, DDRB, _BV(PB1))
#define GPIO_PB2 _PIN_STRUCT(PINB, PORTB, DDRB, _BV(PB2))
#define GPIO_PB3 _PIN_STRUCT(PINB, PORTB, DDRB, _BV(PB3))
#define GPIO_PB4 _PIN_STRUCT(PINB, PORTB, DDRB, _BV(PB4))
#define GPIO_PB5 _PIN_STRUCT(PINB, PORTB, DDRB, _BV(PB5))
#define GPIO_PB6 _PIN_STRUCT(PINB, PORTB, DDRB, _BV(PB6))
#define GPIO_PB7 _PIN_STRUCT(PINB, PORTB, DDRB, _BV(PB7))

#define GPIO_PC0 _PIN_STRUCT(PINC, PORTC, DDRC, _BV(PC0))
#define GPIO_PC1 _PIN_STRUCT(PINC, PORTC, DDRC, _BV(PC1))
#define GPIO_PC2 _PIN_STRUCT(PINC, PORTC, DDRC, _BV(PC2))
#define GPIO_PC3 _PIN_STRUCT(PINC, PORTC, DDRC, _BV(PC3))
#define GPIO_PC4 _PIN_STRUCT(PINC, PORTC, DDRC, _BV(PC4))
#define GPIO_PC5 _PIN_STRUCT(PINC, PORTC, DDRC, _BV(PC5))
#define GPIO_PC6 _PIN_STRUCT(PINC, PORTC, DDRC, _BV(PC6))

#define GPIO_PD0 _PIN_STRUCT(PIND, PORTD, DDRD, _BV(PD0))
#define GPIO_PD1 _PIN_STRUCT(PIND, PORTD, DDRD, _BV(PD1))
#define GPIO_PD2 _PIN_STRUCT(PIND, PORTD, DDRD, _BV(PD2))
#define GPIO_PD3 _PIN_STRUCT(PIND, PORTD, DDRD, _BV(PD3))
#define GPIO_PD4 _PIN_STRUCT(PIND, PORTD, DDRD, _BV(PD4))
#define GPIO_PD5 _PIN_STRUCT(PIND, PORTD, DDRD, _BV(PD5))
#define GPIO_PD6 _PIN_STRUCT(PIND, PORTD, DDRD, _BV(PD6))
#define GPIO_PD7 _PIN_STRUCT(PIND, PORTD, DDRD, _BV(PD7))

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
#define PORT_D0_7 PORTD
#define DDR_D0_7 DDRD
#define PIN_D0_7 PIND
#define D0_MASK _BV(PD0)
#define D1_MASK _BV(PD1)
#define D2_MASK _BV(PD2)
#define D3_MASK _BV(PD3)
#define D4_MASK _BV(PD4)
#define D5_MASK _BV(PD5)
#define D6_MASK _BV(PD6)
#define D7_MASK _BV(PD7)

#define PORT_D8_13 PORTB
#define DDR_D8_13 DDRB
#define PIN_D8_13 PINB
#define D8_MASK _BV(PB0)
#define D9_MASK _BV(PB1)
#define D10_MASK _BV(PB2)
#define D11_MASK _BV(PB3)
#define D12_MASK _BV(PB4)
#define D13_MASK _BV(PB5)

#define PORT_A0_5 PORTC
#define DDR_A0_5 DDRC
#define PIN_A0_5 PINC
#define A0_MASK _BV(PC0)
#define A1_MASK _BV(PC1)
#define A2_MASK _BV(PC2)
#define A3_MASK _BV(PC3)
#define A4_MASK _BV(PC4)
#define A5_MASK _BV(PC5)

/*
GPIO struct based pin functions. Similar implementation to Pololu's FastIO lib for Arduino.
For these to be most efficient values must be known at compile time. 
*/

// Sets pin(s) in struct to input
__attribute__((always_inline)) static inline void GPIO_setInput(GPIO_TypeDef *gpio)
{
    *gpio->pddr &= ~(gpio->pin_mask);
}

// Sets pin(s) in struct to output
__attribute__((always_inline)) static inline void GPIO_setOutput(GPIO_TypeDef *gpio)
{
    *gpio->pddr |= (gpio->pin_mask);
}

// Sets pin(s) direction to value provided. Will both set and clear bits. Value must line up pin_mask that is defined in struct.
__attribute__((always_inline)) static inline void GPIO_setDir(GPIO_TypeDef *gpio, uint8_t dir)
{
    *gpio->pddr = (*gpio->pddr & ~gpio->pin_mask) | dir;
}

// Sets all pins interpreting value as a logical value
__attribute__((always_inline)) static inline void GPIO_setDirLogical(GPIO_TypeDef *gpio, uint8_t dir)
{
    if (dir)
    {
        *gpio->pddr |= (gpio->pin_mask);
    }
    else
    {
        *gpio->pddr &= ~(gpio->pin_mask);
    }
}

// Toggles the pin(s) value. Uses PINx=1 toggle function which should be faster than PORT^=1
__attribute__((always_inline)) static inline void GPIO_toggleValue(GPIO_TypeDef *gpio)
{
    *gpio->ppin = gpio->pin_mask;
}

// Sets the pin value high
__attribute__((always_inline)) static inline void GPIO_setValueHigh(GPIO_TypeDef *gpio)
{
    *gpio->pport |= (gpio->pin_mask);
}

// Set the pin value to low
__attribute__((always_inline)) static inline void GPIO_setValueLow(GPIO_TypeDef *gpio)
{
    *gpio->pport &= ~(gpio->pin_mask);
}

// Sets pin(s) to value provided. Will both set and clear bits. Value must line up pin_mask that is defined in struct.
__attribute__((always_inline)) static inline void GPIO_setValue(GPIO_TypeDef *gpio, uint8_t value)
{
    *gpio->pport = (*gpio->pport & ~gpio->pin_mask) | value;
}

// Sets pin(s) interpreting value as a logical value
__attribute__((always_inline)) static inline void GPIO_setValueLogical(GPIO_TypeDef *gpio, uint8_t value)
{
    if (value)
    {
        *gpio->pport |= (gpio->pin_mask);
    }
    else
    {
        *gpio->pport &= ~(gpio->pin_mask);
    }
}

// Returns the pin(s) input state. Will be logically true if any pin is high.
__attribute__((always_inline)) static inline uint8_t GPIO_getInput(GPIO_TypeDef *gpio)
{
    return (*gpio->ppin & gpio->pin_mask);
}

#endif //GPIO_H