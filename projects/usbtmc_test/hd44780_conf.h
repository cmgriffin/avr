#ifndef __HD44780_CONF__
#define __HD44780_CONF__

#define RS GPIO_D2
#define RW GPIO_D3
#define EN GPIO_D4
// #define D0     GPIO_D12
// #define D1     GPIO_D11
// #define D2     GPIO_D10
// #define D3     GPIO_D9
#define D4     GPIO_A3
#define D5     GPIO_A2
#define D6     GPIO_A1
#define D7     GPIO_A0

#define N_ROWS 2
#define N_COLS 16

#endif // __HD44780_CONF__