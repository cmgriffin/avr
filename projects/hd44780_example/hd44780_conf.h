#ifndef __HD44780_CONF__
#define __HD44780_CONF__

#define RS GPIO_A1
#define RW GPIO_A2
#define EN GPIO_A3
// #define D0     GPIO_D12
// #define D1     GPIO_D11
// #define D2     GPIO_D10
// #define D3     GPIO_D9
#define D4     GPIO_D8
#define D5     GPIO_D7
#define D6     GPIO_D6
#define D7     GPIO_D5

#define N_ROWS 2
#define N_COLS 16

#endif // __HD44780_CONF__