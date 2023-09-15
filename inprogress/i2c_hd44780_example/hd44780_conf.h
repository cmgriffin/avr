#ifndef __HD44780_CONF__
#define __HD44780_CONF__

#define RS GPIO_A0
#define RW GPIO_A1
#define EN GPIO_A2
// #define D0     GPIO_D12
// #define D1     GPIO_D11
// #define D2     GPIO_D10
// #define D3     GPIO_D9
#define D4 GPIO_D8
#define D5 GPIO_D7
#define D6 GPIO_D6
#define D7 GPIO_D5

#define USE_I2C
#define I2C_ADDR 39

#define N_ROWS   4
#define N_COLS   20

#endif // __HD44780_CONF__