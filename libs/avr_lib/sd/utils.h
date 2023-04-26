/*___________________________________________________________________________________________________

Title:
        utils.h v1.0

Description:
        Utility functions used by multiple libraries.

        For complete details visit
        https://www.programming-electronics-diy.xyz

Author:
        Liviu Istrate
        istrateliviu24@yahoo.com

Donate:
        Software development takes time and effort so if you find this useful
consider a small donation at: paypal.me/alientransducer
_____________________________________________________________________________________________________*/

/* ----------------------------- LICENSE - GNU GPL v3
-----------------------------------------------

* This license must be included in any redistribution.

* Copyright (c) 2022 Liviu Istrate, www.programming-electronics-diy.xyz
(istrateliviu24@yahoo.com)

* Project URL: https://www.programming-electronics-diy.xyz

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program. If not, see <https://www.gnu.org/licenses/>.

--------------------------------- END OF LICENSE
--------------------------------------------------*/

#ifndef UTILS_H_
#define UTILS_H_

/*************************************************************
        INCLUDE
**************************************************************/
#include <avrlibdefs.h>

/*************************************************************
        USER DEFINED SETTINGS
**************************************************************/
// Used by functions that write numbers. Select between 32-bit and 64-bit types.
// If you need to display really huge numbers select 64-bit. 32-bit is faster
// though. Maximum number that can be used is 2^32 / 2 or 2^64 / 2 Division by 2
// is because one half is for negative and the other for positive.
typedef int32_t INT_SIZE;   // can be int32_t (default) or int64_t
#define MAX_NR_OF_DIGITS 12 // maximum digits a number can have

/*************************************************************
        FUNCTION PROTOTYPES
**************************************************************/
/*______________________________________________________________________________________________
        Integer to string array conversion.
        String arrays must be defined as:
                char string_integer[MAX_NR_OF_DIGITS + 1];
        This allows itoa() to check if the number of digits fits the array.

        s[]				array pointer for the returned string

        nrOfDigits		Total number of digits desired. If the number to
be displayed has less digits than "nrOfDigits" then it will be padded with
zeros. Useful for maintaining the GUI layout and for displaying floats.
_______________________________________________________________________________________________*/
void STRING_itoa(INT_SIZE n, char s[], int8_t padding);
/*______________________________________________________________________________________________
        Float number to string array conversion. The dot and negative sign are
not added. String arrays must be defined as: char
string_integer[MAX_NR_OF_DIGITS + 1]; char string_float[MAX_NR_OF_DIGITS + 1];
        This allows itoa() to check if the number of digits fits the array.

        s_int[]			array pointer for the integer part
        s_float[]		array pointer for the decimal part
        nrOfDigits		see STRING_itoa()
        decimals		number of digits after the dot
_______________________________________________________________________________________________*/
void STRING_ftoa(float float_nr, char s_int[], char s_float[], uint8_t padding,
                 uint8_t decimals);
/*______________________________________________________________________________________________
        Return the absolute value of i (if the number is negative it will be
converted to positive)
_______________________________________________________________________________________________*/
int MATH_abs(int i);
/*______________________________________________________________________________________________
        Reverse the order of an array

        lenght		length of the array
_______________________________________________________________________________________________*/

void arrayReverse(uint8_t s[], uint8_t lenght);

#endif /* UTILS_H_ */