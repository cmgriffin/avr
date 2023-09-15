#ifndef __HD44780__
#define __HD44780__

#include <avrlibdefs.h>
#include <gpio.h>

/**
 * @brief Initialize IOs to control display then initialize display
 *
 */
void HD44780_init();

/**
 * @brief Clear the display and reset the cursor
 *
 */
void HD44780_clear();

/**
 * @brief Print a char onto the display at the current cursor position
 * @param c byte char to print
 * @param blocking no affect currently, this will always block
 */
bool HD44780_printChar(uint8_t c, bool blocking);

/**
 * @brief Set the cursor to the provided position
 *
 * @param col column to set starting at 0
 * @param row row to set starting at 0
 */
void HD44780_setCursor(uint8_t col, uint8_t row);

/**
 * @brief Custom chracter data definition
 *
 */
typedef struct
{
    uint8_t data[8];
} HD44780_CustChar_t;

/**
 * @brief Store a custom character in the CGRAM of the HD447780
 *
 *
 * @param idx where to store the character in CGRAM, also this is the value used
 * to print the character. 0 through 7 can be used. If 0 is used standard string
 * functions will fail due to it being interpreted as a null instead.
 * @param c instance of HD44780_CustChar_t that contains the character data.
 * This is expected to be stored in PROGMEM
 */
void HD44780_registerChar(uint8_t idx, const HD44780_CustChar_t *c);

#define HD44780_SYM_DEGREE                                                 \
    {                                                                      \
        {                                                                  \
            0B00110, 0B01001, 0B01001, 0B00110, 0B00000, 0B00000, 0B00000, \
                0B00000,                                                   \
        }                                                                  \
    }
#define HD44780_SYM_THERMOMETER                                            \
    {                                                                      \
        {                                                                  \
            0B00100, 0B01010, 0B01010, 0B01010, 0B01010, 0B10001, 0B10001, \
                0B01110,                                                   \
        }                                                                  \
    }
#endif // __HD44780__