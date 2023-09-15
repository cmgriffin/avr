#include "global.h"

#include "devices/hd44780.h"
#if __has_include("hd44780_conf.h")
#include "hd44780_conf.h"
#include <avr/pgmspace.h>
#include <debug_minimal.h>
#include <uart.h>
#include <util/delay.h>

#define DEBUG_ARGFUNC_CALL(func, n) \
    DEBUG_print(func "(");          \
    DEBUG_printint(n);              \
    DEBUG_println(")")
#define DEBUG_FUNC_CALL(func) DEBUG_println(func "()")

#define NUM_COL_PER_ROW       0x40

#if N_ROWS > 2 && N_COLS != 20
#error "Only 16x2 and 20x4 displays are supported"

#endif

#define ROW0_OFFSET 0x00
#define ROW1_OFFSET 0x40
#define ROW2_OFFSET 0x14
#define ROW3_OFFSET 0x54

// Define register bits
#define INSTR_CLR          0x01

#define INSTR_HOME         0x02

#define INSTR_ENTRY_MD_SET 0x04
#define INSTR_INC          0x02
#define INSTR_SHIFT        0x01

#define INSTR_DISP_ONOFF   0x08
#define INSTR_BLINK        0x01
#define INSTR_CURSOR       0x02
#define INSTR_ON           0x04

#define INSTR_SHIFT_MD     0x10
#define INSTR_DISP_SHIFT   0x08
#define INSTR_SHIFT_RIGHT  0x04

#define INSTR_FSET         0x20
#define INSTR_8BIT         0x10
#define INSTR_2LINE        0x08
#define INSTR_10DOTS_HT    0x04

#define INSTR_CGRAM_ADDR   0x40
#define INSTR_DDRAM_ADDR   0x80

#ifndef D3
#define _4BIT_MODE
#else
#define _8BIT_MODE
#endif

// #undef USE_I2C

#ifdef USE_I2C
#include <devices/pfc8574.h>
// define i2c versions of low level functions
#define I2C_RS   _BV(0)
#define I2C_RW   _BV(1)
#define I2C_EN   _BV(2)
#define I2C_BT   _BV(3)
#define I2C_D4   _BV(4)
#define I2C_D5   _BV(5)
#define I2C_D6   _BV(6)
#define I2C_D7   _BV(7)
#define I2C_DATA (0xF0)

// we need to retain the current port configuration
// since everything is handled by the same IO port
uint8_t gCurData;

// some helper defines for setting and clearing bits
#define DATA_CLEAR(d)    (gCurData &= ~(d))
#define DATA_SET(d)      (gCurData |= (d))
#define SEL_INSTR_REG()  DATA_CLEAR(I2C_RS)
#define SEL_DATA_REG()   DATA_SET(I2C_RS)
#define SEL_WRITE()      DATA_CLEAR(I2C_RW)
#define SEL_READ()       DATA_SET(I2C_RW)
#define SET_DATA_INPUT() DATA_SET(I2C_DATA)
#define SET_DATA_OUTPUT(d) \
    DATA_CLEAR(I2C_DATA);  \
    DATA_SET(d &I2C_DATA)

static inline void __pulse_en()
{
    // no delay between is needed since i2c is slowwww
    PFC8574_write(I2C_ADDR, (gCurData |= I2C_EN));
    PFC8574_write(I2C_ADDR, (gCurData &= ~I2C_EN));
}
static inline uint8_t __pulse_en_read()
{
    // no delay between is needed since i2c is slowwww
    // read data is valid while enable is high
    PFC8574_write(I2C_ADDR, (gCurData |= I2C_EN));
    uint8_t data = PFC8574_read(I2C_ADDR) & I2C_DATA;
    PFC8574_write(I2C_ADDR, (gCurData &= ~I2C_EN));
    return data;
}

static void __init_io()
{
    // set the port to known value and enable the backlight
    PFC8574_write(I2C_ADDR, 0x0);
    gCurData = 0;
    PFC8574_write(I2C_ADDR, DATA_SET(I2C_BT));
}

static void __wait()
{
    SEL_INSTR_REG();
    SEL_READ();
    SET_DATA_INPUT();
    PFC8574_write(I2C_ADDR, gCurData);
    uint8_t busy = 0;
    do
    {
        // read upper nibble
        busy = __pulse_en_read() & I2C_D7;
        // read lower nibble even through it isn't needed
        __pulse_en();
    } while (busy);
    // set back to write mode
    // PFC8574_write(I2C_ADDR, DATA_CLEAR(I2C_RW));
}
// #define __wait() _delay_ms(5)

static void __writeInstrReg(uint8_t data)
{
    DEBUG_ARGFUNC_CALL("__writeInstrReg", data);

    __wait();
    SEL_INSTR_REG();
    SEL_WRITE();
    SET_DATA_OUTPUT(data);
    PFC8574_write(I2C_ADDR, gCurData);
    __pulse_en();
    SET_DATA_OUTPUT(data << 4);
    PFC8574_write(I2C_ADDR, gCurData);
    __pulse_en();
}
static void __writeInstrReg_8bit(uint8_t data)
{
    SEL_INSTR_REG();
    SEL_WRITE();
    SET_DATA_OUTPUT(data);
    PFC8574_write(I2C_ADDR, gCurData);
    __pulse_en();
}

static void __writeDataReg(uint8_t data)
{
    DEBUG_ARGFUNC_CALL("__writeDataReg", data);
    __wait();
    SEL_DATA_REG();
    SEL_WRITE();
    SET_DATA_OUTPUT(data);
    PFC8574_write(I2C_ADDR, gCurData);
    __pulse_en();
    SET_DATA_OUTPUT(data << 4);
    PFC8574_write(I2C_ADDR, gCurData);
    __pulse_en();
}
static uint8_t __readInstrReg()
{
    DEBUG_FUNC_CALL("__readInstrReg");
    SEL_INSTR_REG();
    SEL_READ();
    SET_DATA_INPUT();
    PFC8574_write(I2C_ADDR, gCurData); // write to port
    uint8_t data = __pulse_en_read();  // first 4 msb
    data += __pulse_en_read() >> 4;    // last 4 lsb
    return data;
}

static uint8_t __readDataReg()
{
    DEBUG_FUNC_CALL("__readDataReg");
    // select data reg set data to input pullup and read mode
    SEL_DATA_REG();
    SEL_READ();
    SET_DATA_INPUT();
    PFC8574_write(I2C_ADDR, gCurData); // write to port
    uint8_t data = __pulse_en_read();  // first 4 msb
    data += __pulse_en_read() >> 4;    // last 4 lsb
    return data;
}

#else // Use direct GPIO parallel interface
#include <gpio.h>
// define parallel verisons of low level functions
static void __init_io()
{
    GPIO_setValueHigh(&((GPIO_TypeDef)RS));
    GPIO_setValueHigh(&((GPIO_TypeDef)RW));
    GPIO_setValueHigh(&((GPIO_TypeDef)EN));

    GPIO_setOutput(&((GPIO_TypeDef)RS));
    GPIO_setOutput(&((GPIO_TypeDef)RW));
    GPIO_setOutput(&((GPIO_TypeDef)EN));

    __setDataOutput();
}

// Pulse EN high then low for a write only
static void __pulseEn()
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
static void __setDataOutput()
{
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
static void __setDataInput()
{
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
static uint8_t __readDataValue()
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
    return val;
}

// set the data pins to val
static void __setDataValue(uint8_t val)
{
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
}

// wait until the devices has finished processing last command
static void __wait()
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
static void __writeInstrReg(uint8_t instr)
{
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
static void __writeInstrReg_8bit(uint8_t instr)
{
    // __wait();
    __setDataOutput();
    __setDataValue(instr);
    __selInstrReg();
    __pulseEn();
}

// write a value to the data register
static void __writeDataReg(uint8_t data)
{
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
static uint8_t __readInstrReg()
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
    return data;
}

// read a value from the data register
static uint8_t __readDataReg()
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
    return data;
}

#endif // GPIO parallel private functions

// Public Functions
void HD44780_init()
{
    DEBUG_FUNC_CALL("HD44780_init");
    __init_io();

    // intialization routine just in case this didn't happen automatically
    _delay_ms(100);
    __writeInstrReg_8bit(INSTR_FSET | INSTR_8BIT);
    _delay_ms(4.2);
    __writeInstrReg_8bit(INSTR_FSET | INSTR_8BIT);
    _delay_us(110);
    __writeInstrReg_8bit(INSTR_FSET |
                         INSTR_8BIT); // make sure in 8 bit mode first
    // __wait();

#ifdef _8BIT_MODE
    __writeInstrReg(INSTR_FSET | INSTR_8BIT |
                    INSTR_2LINE); // set 8-bit mode, 2-lines, 5x8 font
#elif defined(_4BIT_MODE)
    // special write in 8-bit mode to change to 4-bit
    __writeInstrReg_8bit(INSTR_FSET |
                         INSTR_8BIT); // make sure in 8 bit mode first
    __writeInstrReg_8bit(INSTR_FSET);
    __writeInstrReg(INSTR_FSET |
                    INSTR_2LINE); // set 4-bit mode, 2-lines, 5x8 font
#endif
    __writeInstrReg(INSTR_DISP_ONOFF |
                    INSTR_ON); // display on, cursor off, blink off
    __writeInstrReg(
        INSTR_ENTRY_MD_SET |
        INSTR_INC); // increment and shift cursor, don't shift display
    HD44780_clear();
    HD44780_setCursor(0, 0);
}

void HD44780_clear()
{
    DEBUG_FUNC_CALL("HD44780_clear");
    __writeInstrReg(INSTR_CLR); // clear the display
}

void HD44780_setCursor(uint8_t col, uint8_t row)
{
    uint8_t addr = 0;
    switch (row)
    {
    case 0:
        addr = ROW0_OFFSET;
        break;
#if N_ROWS > 1
    case 1:
        addr = ROW1_OFFSET;
        break;
#endif
#if N_ROWS > 2
    case 2:
        addr = ROW2_OFFSET;
        break;
#endif
#if N_ROWS > 3
    case 3:
        addr = ROW3_OFFSET;
        break;
#endif
    default:
        break;
    }
    __writeInstrReg(INSTR_DDRAM_ADDR + addr + col);
}

void HD44780_registerChar(uint8_t idx, const HD44780_CustChar_t *c)
{
    // make a local copy in memory
    HD44780_CustChar_t tmp;
    memcpy_P(&tmp, c, sizeof(HD44780_CustChar_t));

    // set the starting address in CGRAM
    __writeInstrReg(INSTR_CGRAM_ADDR + idx * 8);
    for (uint8_t i = 0; i < 8; i++)
    {
        __writeDataReg(tmp.data[i]);
    }
    // __writeDataReg(0); // blank line for cursor
    // set addr back to zero for prining characters
    __writeInstrReg(INSTR_DDRAM_ADDR);
}

bool HD44780_printChar(uint8_t c, bool blocking)
{
    DEBUG_ARGFUNC_CALL("HD44780_printChar", c);
    // assumes address has not been changed and DDRAM is still selected
    __writeDataReg(c);
    return true;
}

bool HD44780_printCharScrolling(uint8_t c, bool blocking)
{
    if (c == '\n')
    {
        HD44780_setCursor(0, N_ROWS - 1); // set the cursor to bottom row
        // copy bottom row to top row
        uint8_t i, b[N_COLS];
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