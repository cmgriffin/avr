#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stddef.h>
#include <stdio.h>
#include <util/delay.h>
#include <gpio.h>
#include <uart.h>
#include <serial_commands.h>

GPIO_TypeDef led_pin = D13;

// // Define some commands
void callback0(char *args);
void callback1(char *args);

const char cmd_str0[] PROGMEM = "A";
const char cmd_str1[] PROGMEM = "B";
const char delim[] PROGMEM = " ";
const serial_command_t sCmd PROGMEM = {
    .delimiter = (char *)delim,
    .n_cmds = 2,
    .cmds = {
        {.callback = callback0, .cmd_str = (char *)cmd_str0},
        {.callback = callback1, .cmd_str = (char *)cmd_str1}}};

int main(void)
{

  UART_init();
  UART_printStr_p(PSTR("Program Start!\n"));

  for (;;)
  {
    char inputBuff[10], outputBuff[32];
    UART_readString(inputBuff, 10, 1);
    sprintf(outputBuff, "input received: %s\n", inputBuff);
    UART_printStr(outputBuff);

    handleSerialCommands(&sCmd, inputBuff);
  }

  return 0;
}

void callback0(char *args)
{
  UART_printStr_p(PSTR("callback0 executed\n"));
  // parse_print_args(args);
}
void callback1(char *args)
{
  UART_printStr_p(PSTR("callback1 executed\n"));
  // parse_print_args(args);
}

// void parse_print_args(char *args)
// {
//   uint8_t i = 0;
//   char s[3];
//   char *arg;
//   arg = strtok(args, DELIM);
//   while (arg != NULL)
//   {
//     UART_printStr_p(PSTR("ARG #"));
//     UART_printStr(itoa(i, s, 10));
//     UART_printStr_p(PSTR(": "));
//     UART_printStr(arg);
//     UART_printStr_p(PSTR("\n"));
//     i++;
//     arg = strtok(NULL, DELIM);
//   }
// }