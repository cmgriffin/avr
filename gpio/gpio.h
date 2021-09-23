#ifndef GPIO_H
#define GPIO_H

#include <avr/io.h>

// Define a GPIO type hold all information neccessary for accessing a GPIO

typedef struct
{
    volatile uint8_t *ppin;
    volatile uint8_t *pport;
    volatile uint8_t *pddr;
    uint8_t pin;
} GPIO_TypeDef;

#define _PIN_STRUCT(pin, port, ddr, bit) \
    {                                    \
        &pin, &port, &ddr, bit           \
    }

#define GPIO_PB0 _PIN_STRUCT(PINB, PORTB, DDRB, PB0)
#define GPIO_PB1 _PIN_STRUCT(PINB, PORTB, DDRB, PB1)
#define GPIO_PB2 _PIN_STRUCT(PINB, PORTB, DDRB, PB2)
#define GPIO_PB3 _PIN_STRUCT(PINB, PORTB, DDRB, PB3)
#define GPIO_PB4 _PIN_STRUCT(PINB, PORTB, DDRB, PB4)
#define GPIO_PB5 _PIN_STRUCT(PINB, PORTB, DDRB, PB5)
#define GPIO_PB6 _PIN_STRUCT(PINB, PORTB, DDRB, PB6)
#define GPIO_PB7 _PIN_STRUCT(PINB, PORTB, DDRB, PB7)

#define GPIO_PC0 _PIN_STRUCT(PINC, PORTC, DDRC, PC0)
#define GPIO_PC1 _PIN_STRUCT(PINC, PORTC, DDRC, PC1)
#define GPIO_PC2 _PIN_STRUCT(PINC, PORTC, DDRC, PC2)
#define GPIO_PC3 _PIN_STRUCT(PINC, PORTC, DDRC, PC3)
#define GPIO_PC4 _PIN_STRUCT(PINC, PORTC, DDRC, PC4)
#define GPIO_PC5 _PIN_STRUCT(PINC, PORTC, DDRC, PC5)
#define GPIO_PC6 _PIN_STRUCT(PINC, PORTC, DDRC, PC6)
#define GPIO_PC7 _PIN_STRUCT(PINC, PORTC, DDRC, PC7)

#define GPIO_PD0 _PIN_STRUCT(PIND, PORTD, DDRD, PD0)
#define GPIO_PD1 _PIN_STRUCT(PIND, PORTD, DDRD, PD1)
#define GPIO_PD2 _PIN_STRUCT(PIND, PORTD, DDRD, PD2)
#define GPIO_PD3 _PIN_STRUCT(PIND, PORTD, DDRD, PD3)
#define GPIO_PD4 _PIN_STRUCT(PIND, PORTD, DDRD, PD4)
#define GPIO_PD5 _PIN_STRUCT(PIND, PORTD, DDRD, PD5)
#define GPIO_PD6 _PIN_STRUCT(PIND, PORTD, DDRD, PD6)
#define GPIO_PD7 _PIN_STRUCT(PIND, PORTD, DDRD, PD7)

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

__attribute__((always_inline)) static inline void GPIO_setInput(GPIO_TypeDef *gpio)
{
    *gpio->pddr &= ~(1 << gpio->pin);
}

__attribute__((always_inline)) static inline void GPIO_setOutput(GPIO_TypeDef *gpio)
{
    *gpio->pddr |= (1 << gpio->pin);
}

__attribute__((always_inline)) static inline void GPIO_setDir(GPIO_TypeDef *gpio, uint8_t dir)
{
    if (dir)
    {
        *gpio->pddr |= (1 << gpio->pin);
    }
    else
    {
        *gpio->pddr &= ~(1 << gpio->pin);
    }
}

__attribute__((always_inline)) static inline void GPIO_toggle(GPIO_TypeDef *gpio)
{
    *gpio->pport ^= (1 << gpio->pin);
}

__attribute__((always_inline)) static inline void GPIO_setValueHigh(GPIO_TypeDef *gpio)
{
    *gpio->pport |= (1 << gpio->pin);
}

__attribute__((always_inline)) static inline void GPIO_setValueLow(GPIO_TypeDef *gpio)
{
    *gpio->pport &= ~(1 << gpio->pin);
}

__attribute__((always_inline)) static inline void GPIO_setValue(GPIO_TypeDef *gpio, uint8_t value)
{
    if (value)
    {
        *gpio->pport |= (1 << gpio->pin);
    }
    else
    {
        *gpio->pport &= ~(1 << gpio->pin);
    }
}

__attribute__((always_inline)) static inline uint8_t GPIO_getInput(GPIO_TypeDef *gpio)
{
    return *gpio->ppin >> gpio->pin & 1;
}

#endif //GPIO_H