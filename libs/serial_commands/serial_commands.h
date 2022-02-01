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
typedef void (*callback_t)(char *args);
typedef struct cmd
{
    callback_t callback;
    char *cmd_str;
} cmd_t;

typedef struct serial_command
{
    char *delimiter;
    uint8_t n_cmds;
    cmd_t *cmds;

} serial_command_t;

void handleSerialCommands(const serial_command_t *sCmd, char *cmd);

#endif // SERIAL_COMMANDS_H