#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H

/* 
* Implements serial callback commands
* When the handleSerialCommands function is called it is passed a string pointer having the command to be parsed.
* The list of command strings is looped through and the matching command is found.
* The corresponding callback function is then called passing the remaining arguments.
* The responsibility is given to the callback function to parse out and handle the arguments
*/

#define DEBUG

#define DELIM " "

#define CMD_0 "A"
#define CMD_1 "B"
#define CMD_2 "C"
#define CMD_3 "D"

void callback0(char *args);
void callback1(char *args);
void callback2(char *args);
void callback3(char *args);

void handleSerialCommands(char *cmd);

void parse_print_args(char *args);

#endif // SERIAL_COMMANDS_H