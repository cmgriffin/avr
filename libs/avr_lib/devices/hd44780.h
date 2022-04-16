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
 * newline char causes the display to go to next line
 * if line overflows it will continue on the next line
 * @param c byte char to print
 * @param blocking no affect currently, this will always block
 */
void HD44780_printChar(uint8_t c, bool blocking);

/**
 * @brief Set the cursor to the provided position
 *
 * @param col column to set starting at 0
 * @param row row to set starting at 0
 */
void HD44780_setCursor(uint8_t col, uint8_t row);

#endif // __HD44780__