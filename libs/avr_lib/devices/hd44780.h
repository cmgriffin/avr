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
bool HD44780_printChar(uint8_t c, bool blocking);

/**
 * @brief A scrolling version of the printChar function
 * The current line is stored in a buffer until a newline is detected. This new
 * line is then inserted into the bottom row an all remaining lines are shifted
 * up. Adds some overhead due to reading back and copying lines from one row to
 * another.
 *
 * @param c byte char to print
 * @param blocking no affect currently, this will always block
 */
bool HD44780_printCharScrolling(uint8_t c, bool blocking);

/**
 * @brief Set the cursor to the provided position
 *
 * @param col column to set starting at 0
 * @param row row to set starting at 0
 */
void HD44780_setCursor(uint8_t col, uint8_t row);

#endif // __HD44780__