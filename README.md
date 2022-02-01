# AVR
Collection of libraries and small projects from working with AVR microcontrollers. 
With libaries that are hardware independant an effort was made to keep them compatible with Arduino.

* gpio.h - a struct based collection of defines and functions for working the AVR GPIO
    * Configuring pin(s) direction: GPIO_setdir, GPIO_setDirLogical, GPIO_setDir, GPIO_setInput, GPIO_setOutput
    * Setting pin(s) values: GPIO_setValue, GPIO_setValueLogical, GPIO_setValueHigh, GPIO_setValueLow, GPIO_toggleValue
    * Getting pin(s) values: GPIO_getInput
* print.h - a collection of functions for printing strings
    * printStr - print a string from RAM to the provided sendChar function
    * printStr_p - print a string from flash to the provided sendChar function
    * print_uint16 - print a uint16 as a decimal with specified justification and decimals
    * print_int16 - same as above but with a negative sign where necessary
* uart.h - a collection of functions to use the AVR UART hardware
* serial_commands.h - a simple serial command handler where all data is stored in flash
