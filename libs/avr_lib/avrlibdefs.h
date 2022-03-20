/**
 * @file avrlibdefs.h
 * @author your name (you@domain.com)
 * @brief Definitions to be included for all source files
 * @version 0.1
 * @date 2022-03-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __AVRLIBDEFS__
#define __AVRLIBDEFS__

#include <avr/io.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief set and/or clear bits in masked region to match val
 *
 */
#define update_bits(sfr, val, mask) (sfr) = (~(mask) & (sfr)) | ((val) & (mask))

#endif // __AVRLIBDEFS__