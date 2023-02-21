#include "global.h"
#include <avr/pgmspace.h>
#include <stdio.h>
#include <uart.h>
#include <util/delay.h>

#define printp(fmt, args...) printf_P(PSTR("" fmt "\n"), args)

const char str0[] PROGMEM  = "Hello World0!\n";
const char str1[] PROGMEM  = "Hello World1!\n";
const char str2[] PROGMEM  = "Hello World2!\n";
const char str3[] PROGMEM  = "Hello World3!\n";
const char str4[] PROGMEM  = "Hello World4!\n";

const char label[] PROGMEM = "list0";
// const char *const strs[] PROGMEM = {str0, str1, str2, str3, str4};

typedef struct node
{
    const char *str;
    uint8_t n;
} node;

typedef struct list
{
    uint8_t n_nodes;
    const char *label;
    const node *nodes;
} list;

const node nodes[] PROGMEM = {
    {str0, 0}, {str1, 1}, {str2, 2}, {str3, 3}, {str4, 4}};

const list l PROGMEM = {.n_nodes = 5, .label = label, .nodes = nodes};
const list *lptr     = &l;
int main(void)
{
    UART_init();
    // access a byte in the base struct
    printp("read n_nodes: %d", (uint8_t)pgm_read_byte(&(lptr->n_nodes)));
    // read the label string
    printp("value of the label string: ", 0);
    printf_P((PGM_P)pgm_read_word(&(lptr->label)));
    printp("", 0);

    // from the inner stuct
    // get the pointer to the list from the upper struct
    const node *n = (const node *)pgm_read_word(&lptr->nodes);
    for (uint8_t i = 0; i < pgm_read_byte(&(lptr->n_nodes)); i++)
    {
        printp("read n of node %d: %d", i, (uint8_t)pgm_read_byte(&(n[i].n)));
        printp("read str of node %d: %S", i, (PGM_P)pgm_read_word(&(n[i].str)));
    }

    // for (uint8_t i = 0; i < 5; i++)
    // {
    //     printf_P(strs[i]);
    // }

    return 0;
}
