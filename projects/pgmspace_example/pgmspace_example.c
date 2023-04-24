#include "global.h"
#include <avr/pgmspace.h>
#include <stdio.h>
#include <uart.h>
// #include <util/delay.h>

#define print(fmt, ...) printf_P(PSTR(fmt), ##__VA_ARGS__)

typedef struct
{
    const uint8_t type_num;
    const int8_t exp;
    const int16_t default_val;
    const int16_t min_val;
    const int16_t max_val;
    const char name[20];
} menuItem_t;

// const menuItem_t item0 PROGMEM        = {0, 0, 10, 0, 100, "MenuItem0"};
// const menuItem_t item1 PROGMEM        = {0, -1, 10, 0, 100, "MenuItem1"};
// const menuItem_t item2 PROGMEM        = {0, -2, 10, 0, 100, "MenuItem2"};
// const menuItem_t item3 PROGMEM        = {0, 1, 10, 0, 100, "MenuItem3"};
// const menuItem_t item4 PROGMEM        = {0, 2, 10, 0, 100, "MenuItem4"};

const menuItem_t PROGMEM menu_items[] = {{0, 0, 10, 0, 100, "MenuItem0"},
                                         {0, -1, 10, 0, 100, "MenuItem1"}};

int main(void)
{
    UART_init();
    // // access a byte in the base struct
    // printp("read n_nodes: %d", (uint8_t)pgm_read_byte(&(lptr->n_nodes)));
    // // read the label string
    // printp("value of the label string: ", 0);
    // printf_P((PGM_P)pgm_read_word(&(lptr->label)));
    // printp("", 0);

    // // from the inner stuct
    // // get the pointer to the list from the upper struct
    // const node *n = (const node *)pgm_read_word(&lptr->nodes);

    char str_buff[20];

    for (uint8_t i = 0; i < (sizeof(menu_items) / sizeof(menu_items[0])); i++)
    {
        // strcpy_P(str_buff, menu_items[i].name);
        print("%d name: %s, type_num: %d, exp: %d, default_val: %d, min_val: "
              "%d, max_val:%d\n",
              i, strcpy_P(str_buff, menu_items[i].name),
              pgm_read_byte(&(menu_items[i].type_num)),
              pgm_read_byte(&(menu_items[i].exp)),
              pgm_read_byte(&(menu_items[i].default_val)),
              pgm_read_byte(&(menu_items[i].min_val)),
              pgm_read_byte(&(menu_items[i].max_val)));
    }

    // for (uint8_t i = 0; i < pgm_read_byte(&(lptr->n_nodes)); i++)
    // {
    //     printp("read n of node %d: %d", i,
    //     (uint8_t)pgm_read_byte(&(n[i].n))); printp("read str of node %d: %S",
    //     i, (PGM_P)pgm_read_word(&(n[i].str)));
    // }

    // for (uint8_t i = 0; i < 5; i++)
    // {
    //     printf_P(strs[i]);
    // }

    return 0;
}
