#include "global.h"

#include "devices/hd44780.h"
#if __has_include("hd44780_conf.h")
#include "hd44780_conf.h"
#include <gpio.h>
#include <util/delay.h>

//#define DEBUG_SHORT
#include <debug.h>

#define CLEAR                  0x01
#define HOME                   0x02

#define ENTRYMD_INC_CURSOR     (0x02 | 0x04)
#define ENTRYMD_DEC_CURSOR     (0x00 | 0x04)
#define ENTRYMD_FOLLOW_SHIFT   (0x01 | 0x04)
#define ENTRYMD_NOFOLLOW_SHIFT (0x00 | 0x04)

#define ONOFF_DISP_OFF         (0x00 | 0x08)
#define ONOFF_DISP_ON          (0x04 | 0x08)
#define ONOFF_CURSOR_OFF       (0x00 | 0x08)
#define ONOFF_CURSOR_ON        (0x02 | 0x08)
#define ONOFF_CURSOR_NOBLINK   (0x00 | 0x08)
#define ONOFF_CURSOR_BLINK     (0x01 | 0x08)

#define SHIFT_DISP_SHIFT       (0x08 | 0x10)
#define SHIFT_CURSOR_SHIFT     (0x00 | 0x10)
#define SHIFT_SHIFT_RIGHT      (0x04 | 0x10)
#define SHIFT_SHIFT_LEFT       (0x00 | 0x10)

#define FUNC_EIGHTBITS         (0x10 | 0x20)
#define FUNC_FOURBITS          (0x00 | 0x20)
#define FUNC_TWOLINE           (0x08 | 0x20)
#define FUNC_ONELINE           (0x00 | 0x20)
#define FUNC_CHAR5X10          (0x04 | 0x20)
#define FUNC_CHAR5X8           (0x00 | 0x20)

#define SET_CGRAM_ADDR         0x40
#define SET_DDRAM_ADDR         0x80

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
        _delay_us(1);
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
    __selDataReg();
    __pulseEn();
#ifdef _4BIT_MODE
    __setDataValue(data << 4);
    __selDataReg();
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
    __wait();
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

    // intialization routine just in case this didn't happen automatically
    __writeInstrReg_8bit(FUNC_EIGHTBITS);
    _delay_ms(4.2);
    __writeInstrReg_8bit(FUNC_EIGHTBITS);
    _delay_us(110);
    __writeInstrReg_8bit(FUNC_EIGHTBITS); // make sure in 8 bit mode first
    __wait();

#ifdef _8BIT_MODE
    __writeInstrReg(FUNC_EIGHTBITS | FUNC_TWOLINE |
                    FUNC_CHAR5X8); // set 8-bit mode, 2-lines, 5x8 font
#elif defined(_4BIT_MODE)
    // special write in 8-bit mode to change to 4-bit
    // since we can't do the wait we make sure sure the display is actually
    // in 8-bit mode before going to 4 bit mode
    __writeInstrReg_8bit(FUNC_FOURBITS); // just set to 4-bit mode first
    __writeInstrReg(FUNC_FOURBITS | FUNC_TWOLINE | FUNC_CHAR5X8);
#endif
    __writeInstrReg(ONOFF_DISP_ON | ONOFF_CURSOR_OFF | ONOFF_CURSOR_NOBLINK);
    __writeInstrReg(ENTRYMD_INC_CURSOR | ENTRYMD_NOFOLLOW_SHIFT);
    __writeInstrReg(CLEAR); // clear the display
}

void HD44780_clear()
{
    __writeInstrReg(CLEAR); // clear the display
}

void HD44780_setCursor(uint8_t col, uint8_t row)
{
    __writeInstrReg(SET_DDRAM_ADDR + (row ? 0x40 : 0) + col);
}

bool HD44780_printChar(uint8_t c, bool blocking)
{
    _DEBUG("HD44780_printChar(%c)", c);
    if (c == '\n')
    {
        __writeInstrReg(SET_DDRAM_ADDR + 0x40); // go to next line
        return true;
    }
    if (c == '\r')
    {
        return true;
    }
    __writeDataReg(c);

    if ((__readInstrReg() & 0x7f) == 16)
    {
        __writeInstrReg(SET_DDRAM_ADDR + 0x40); // go to next line
    }
    return true;
}

bool HD44780_printCharScrolling(uint8_t c, bool blocking)
{
    _DEBUG("HD44780_printChar(%c)", c);
    static uint8_t col, newLine[N_COLS];
    if (c == '\n')
    {
        uint8_t i, b[N_COLS];
        for (i = col; i < N_COLS; i++)
        {
            newLine[i] = ' ';
        }
        col = 0;
        HD44780_setCursor(0, N_ROWS - 1); // set the cursor to bottom row
        // copy bottom row to top row

        for (i = 0; i < N_COLS; i++)
        {
            b[i] = __readDataReg();
        }
        HD44780_setCursor(0, 0); // set the cursor to the top row
        for (i = 0; i < N_COLS; i++)
        {
            __writeDataReg(b[i]);
        }
        HD44780_setCursor(0, N_ROWS - 1); // set the cursor to bottom row
        for (i = 0; i < N_COLS; i++)
        {
            __writeDataReg(newLine[i]);
        }
        return true;
    }
    if (c == '\r')
    {
        // unused but could possible be used to reset the cursor to the start of
        // the line
        return true;
    }
    newLine[col++] = c;

    if (col == N_COLS)
    {
        HD44780_printCharScrolling('\n', true);
    }
    return true;
}

#endif