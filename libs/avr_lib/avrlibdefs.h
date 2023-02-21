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

#define STRING2(x)                  #x
#define STRING(x)                   STRING2(x)
#define MIN(x, y)                   (((x) < (y)) ? (x) : (y))
#define MAX(x, y)                   (((x) > (y)) ? (x) : (y))

/**
 * @brief Handy macros courtesy of LUFA Library
 *    Copyright (C) Dean Camera, 2021.
 *
 */
/** Indicates to the compiler that the function can not ever return, so that any
 * stack restoring or return code may be omitted by the compiler in the
 * resulting binary.
 */
#define ATTR_NO_RETURN __attribute__((noreturn))
/** Forces the compiler to inline the specified function. When applied, the
 * given function will be in-lined under all circumstances.
 */
#define ATTR_ALWAYS_INLINE __attribute__((always_inline))
/** Marks a variable or struct element for packing into the smallest space
 * available, omitting any alignment bytes usually added between fields to
 * optimize field accesses.
 */
#define ATTR_PACKED __attribute__((packed))
/** Marks a function as a weak reference, which can be overridden by other
 * functions with an identical name (in which case the weak reference is
 * discarded at link time).
 */
#define ATTR_WEAK __attribute__((weak))
/** Marks a function as an alias for another function.
 *
 *  \param[in] Func  Name of the function which the given function name should
 * alias.
 */
#define ATTR_ALIAS(Func) __attribute__((alias(#Func)))

#endif // __AVRLIBDEFS__