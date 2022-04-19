#include "global.h"

#include "devices/hd44780.h"
#if __has_include("hd44780_conf.h")
#include "hd44780_conf.h"
#include <gpio.h>
#include <util/delay.h>

//#define DEBUG_SHORT
#include <debug.h>

#ifndef D3
#define _4BIT_MODE
#else
#define _8BIT_MODE
#endif

// Private Functions

// Low Level functions

// Pulse EN high then low for a write only
void __pulseEn()
{
    GPIO_setValueHigh(&((GPIO_TypeDef)EN));
    _delay_us(1);
    GPIO_setValueLow(&((GPIO_TypeDef)EN));
    _delay_us(1);
}

// select the instruction register for writing/reading
static inline void __selInstrReg() { GPIO_setValueLow(&((GPIO_TypeDef)RS)); }

// select the data register for writing/reading
static inline void __selDataReg() { GPIO_setValueHigh(&((GPIO_TypeDef)RS)); }

// configure for a register read
static inline void __setRead() { GPIO_setValueHigh(&((GPIO_TypeDef)RW)); }

// configure for a register write
static inline void __setWrite() { GPIO_setValueLow(&((GPIO_TypeDef)RW)); }

// set the data pins to output
void __setDataOutput()
{
    _DEBUG("__setDataOutput()", 0);
    __setWrite(); // set the device to write mode
    GPIO_setOutput(&((GPIO_TypeDef)D7));
    GPIO_setOutput(&((GPIO_TypeDef)D6));
    GPIO_setOutput(&((GPIO_TypeDef)D5));
    GPIO_setOutput(&((GPIO_TypeDef)D4));

#ifdef _8BIT_MODE
    GPIO_setOutput(&((GPIO_TypeDef)D3));
    GPIO_setOutput(&((GPIO_TypeDef)D2));
    GPIO_setOutput(&((GPIO_TypeDef)D1));
    GPIO_setOutput(&((GPIO_TypeDef)D0));
#endif
}

// set the data pins to input
void __setDataInput()
{
    _DEBUG("__setDataInput()", 0);
    GPIO_setInput(&((GPIO_TypeDef)D7));
    GPIO_setInput(&((GPIO_TypeDef)D6));
    GPIO_setInput(&((GPIO_TypeDef)D5));
    GPIO_setInput(&((GPIO_TypeDef)D4));

#ifdef _8BIT_MODE
    GPIO_setInput(&((GPIO_TypeDef)D3));
    GPIO_setInput(&((GPIO_TypeDef)D2));
    GPIO_setInput(&((GPIO_TypeDef)D1));
    GPIO_setInput(&((GPIO_TypeDef)D0));
#endif
    __setRead();
}

// read the data currently on the data pins
uint8_t __readDataValue()
{
    uint8_t val = 0;
    val += GPIO_getInput(&((GPIO_TypeDef)D7)) ? 0x80 : 0;
    val += GPIO_getInput(&((GPIO_TypeDef)D6)) ? 0x40 : 0;
    val += GPIO_getInput(&((GPIO_TypeDef)D5)) ? 0x20 : 0;
    val += GPIO_getInput(&((GPIO_TypeDef)D4)) ? 0x10 : 0;

#ifdef _8BIT_MODE
    val += GPIO_getInput(&((GPIO_TypeDef)D3)) ? 0x8 : 0;
    val += GPIO_getInput(&((GPIO_TypeDef)D2)) ? 0x4 : 0;
    val += GPIO_getInput(&((GPIO_TypeDef)D1)) ? 0x2 : 0;
    val += GPIO_getInput(&((GPIO_TypeDef)D0)) ? 0x1 : 0;
#endif
    _DEBUG("__readDataValue() = %X", val);
    return val;
}

// set the data pins to val
void __setDataValue(uint8_t val)
{
    _DEBUG("__setDataValue(%X)", val);
    GPIO_setValueLogical(&((GPIO_TypeDef)D7), val & 0x80);
    GPIO_setValueLogical(&((GPIO_TypeDef)D6), val & 0x40);
    GPIO_setValueLogical(&((GPIO_TypeDef)D5), val & 0x20);
    GPIO_setValueLogical(&((GPIO_TypeDef)D4), val & 0x10);

#ifdef _8BIT_MODE
    GPIO_setValueLogical(&((GPIO_TypeDef)D3), val & 0x8);
    GPIO_setValueLogical(&((GPIO_TypeDef)D2), val & 0x4);
    GPIO_setValueLogical(&((GPIO_TypeDef)D1), val & 0x2);
    GPIO_setValueLogical(&((GPIO_TypeDef)D0), val & 0x1);
#endif
    _DEBUG("__setDataValue(%X), check=%X", val, __readDataValue());
}

// wait until the devices has finished processing last command
void __wait()
{
    __setDataInput();
    __selInstrReg();
    uint8_t busy;
    do
    {
        _delay_us(1);
        GPIO_setValueLow(&((GPIO_TypeDef)EN));
        _delay_us(1);
        GPIO_setValueHigh(&((GPIO_TypeDef)EN));
        _delay_us(1);
        busy = GPIO_getInput(&((GPIO_TypeDef)D7));
#ifdef _4BIT_MODE
        GPIO_setValueLow(&((GPIO_TypeDef)EN));
        _delay_us(1);
        GPIO_setValueHigh(&((GPIO_TypeDef)EN));
#endif
    } while (busy);

    GPIO_setValueLow(&((GPIO_TypeDef)EN));
    __setDataOutput();
}

// write to the instuction register
void __writeInstrReg(uint8_t instr)
{
    _DEBUG("__writeInstrReg(%x)", instr);
    __wait(); // wait for previous command to complete, also sets data to
              // output/write
    __setDataValue(instr);
    __pulseEn();
#ifdef _4BIT_MODE
    __setDataValue(instr << 4);
    __selInstrReg();
    __pulseEn();
#endif
}

// write to the instruction register in 8bit mode regardless of configuration
// this is used for first init steps to enable 4bit mode
void __writeInstrReg_8bit(uint8_t instr)
{
    _DEBUG("__writeInstrReg(%x)", instr);
    // __wait();
    __setDataOutput();
    __setDataValue(instr);
    __selInstrReg();
    __pulseEn();
}

// write a value to the data register
void __writeDataReg(uint8_t data)
{
    _DEBUG("__writeDataReg(%x)", data);
    __wait();
    __setDataValue(data);
    __selInstrReg();
    __pulseEn();
#ifdef _4BIT_MODE
    __setDataValue(data << 4);
    __selInstrReg();
    __pulseEn();
#endif
}

// read value from an Read busy flag & address register
uint8_t __readInstrReg()
{
    __setDataInput();
    __selInstrReg();
    GPIO_setValueLow(&((GPIO_TypeDef)EN));
    _delay_us(1);
    GPIO_setValueHigh(&((GPIO_TypeDef)EN));
    _delay_us(1);
    uint8_t data = __readDataValue();
#ifdef _4BIT_MODE
    GPIO_setValueLow(&((GPIO_TypeDef)EN));
    _delay_us(1);
    GPIO_setValueHigh(&((GPIO_TypeDef)EN));
    _delay_us(1);
    data += __readDataValue() >> 4;
#endif
    GPIO_setValueLow(&((GPIO_TypeDef)EN));
    _DEBUG("__readInstrReg() = %x", data);
    return data;
}

// read a value from the data register
uint8_t __readDataReg()
{
    __setDataInput();
    __selDataReg();
    GPIO_setValueLow(&((GPIO_TypeDef)EN));
    _delay_us(1);
    GPIO_setValueHigh(&((GPIO_TypeDef)EN));
    _delay_us(1);
    uint8_t data = __readDataValue();
#ifdef _4BIT_MODE
    GPIO_setValueLow(&((GPIO_TypeDef)EN));
    _delay_us(1);
    GPIO_setValueHigh(&((GPIO_TypeDef)EN));
    _delay_us(1);
    data += __readDataValue() >> 4;
#endif
    GPIO_setValueLow(&((GPIO_TypeDef)EN));
    _DEBUG("__readDataReg() = %x", data);
    return data;
}

// Public Functions
void HD44780_init()
{
    _DEBUG("HD44780_init()", 0);
    GPIO_setValueHigh(&((GPIO_TypeDef)RS));
    GPIO_setValueHigh(&((GPIO_TypeDef)RW));
    GPIO_setValueHigh(&((GPIO_TypeDef)EN));

    GPIO_setOutput(&((GPIO_TypeDef)RS));
    GPIO_setOutput(&((GPIO_TypeDef)RW));
    GPIO_setOutput(&((GPIO_TypeDef)EN));

    __setDataOutput();

#ifdef _8BIT_MODE
    __writeInstrReg(0x38); // set 8-bit mode, 2-lines, 5x8 font
#elif defined(_4BIT_MODE)
    // special write in 8-bit mode to change to 4-bit
    __writeInstrReg_8bit(0x30); // make sure in 8 bit mode first
    __writeInstrReg_8bit(0x20);
    __writeInstrReg(0x28); // set 4-bit mode, 2-lines, 5x8 font
#endif
    __writeInstrReg(0x0C); // display on, cursor off, blink off
    __writeInstrReg(0x06); // increment and shift cursor, don't shift display
    __writeInstrReg(0x01); // clear the display
}

void HD44780_clear()
{
    __writeInstrReg(0x01); // clear the display
}

void HD44780_setCursor(uint8_t col, uint8_t row)
{
    __writeInstrReg(0x80 + (row ? 0x40 : 0) + col);
}

bool HD44780_printChar(uint8_t c, bool blocking)
{
    _DEBUG("HD44780_printChar(%c)", c);
    if (c == '\n')
    {
        __writeInstrReg(0xC0); // go to next line
        return true;
    }
    if (c == '\r')
    {
        return true;
    }
    __writeDataReg(c);

    if ((__readInstrReg() & 0x7f) == 16)
    {
        __writeInstrReg(0xC0); // go to next line
    }
    return true;
}

bool HD44780_printCharScrolling(uint8_t c, bool blocking)
{
    _DEBUG("HD44780_printChar(%c)", c);
    if (c == '\n')
    {
        HD44780_setCursor(0, N_ROWS - 1); // set the cursor to bottom row
        // copy bottom row to top row
        uint8_t i, b[N_COLS];
        for (i = 0, i < N_COLS : i++)
        {
            b[i] = __readDataReg();
        }
        HD44780_setCursor(0, 0); // set the cursor to the top row
        for (i = 0, i < N_COLS : i++)
        {
            __writeDataReg(b[i]);
        }
        HD44780_setCursor(0, N_ROWS - 1); // set the cursor to bottom row
        return true;
    }
    if (c == '\r')
    {
        return true;
    }
    __writeDataReg(c);

    if ((__readInstrReg() & 0x7f) == N_COLS)
    {
        HD44780_printCharScrolling('\n', true);
    }
    return true;
}

#endif