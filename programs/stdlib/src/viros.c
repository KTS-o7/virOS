/*
    ? viros_parse_command:
    * This function takes in a command string and a maximum length,
    * and it returns a linked list of arguments.
    * It tokenizes the command string using spaces as delimiters
    * and creates a new struct for each token.

    ? viros_getkeyblock:
    * This function blocks until a key is pressed
    * and then it returns the key's value.

    ? viros_terminal_readline:
    * This function reads a line of input from the terminal,
    * with the option to output the characters as they are typed.
    * The input is stored in an output buffer
    * and the function returns when the user presses the Enter key.

    ? viros_system_run:
    * This function takes a command string, parses it into arguments,
    * and then runs the command using the viros_system function.
*/

#include "viros.h"
#include "string.h"

// Parse the command into a linked list of arguments
struct command_argument *viros_parse_command(const char *command, int max)
{
    struct command_argument *root_command = 0;
    char scommand[1025];
    if (max >= (int)sizeof(scommand))
    {
        return 0;
    }

    strncpy(scommand, command, sizeof(scommand));
    char *token = strtok(scommand, " ");
    if (!token)
    {
        goto out;
    }

    root_command = viros_malloc(sizeof(struct command_argument));
    if (!root_command)
    {
        goto out;
    }

    strncpy(root_command->argument, token, sizeof(root_command->argument));
    root_command->next = 0;

    struct command_argument *current = root_command;
    token = strtok(NULL, " ");
    while (token != 0)
    {
        struct command_argument *new_command = viros_malloc(sizeof(struct command_argument));
        if (!new_command)
        {
            break;
        }

        strncpy(new_command->argument, token, sizeof(new_command->argument));
        new_command->next = 0x00;
        current->next = new_command;
        current = new_command;
        token = strtok(NULL, " ");
    }

out:
    return root_command;
}

// Block until a key is pressed
int viros_getkeyblock()
{
    int val = 0;
    do
    {
        val = viros_getkey();
    } while (val == 0);

    return val;
}

// Read a line from the terminal
void viros_terminal_readline(char *out, int max, bool output_while_typing)
{
    int i = 0;
    for (i = 0; i < max - 1; i++)
    {
        char key = viros_getkeyblock();

        // Carriage return means we have read the line
        if (key == 13)
        {
            break;
        }

        if (output_while_typing)
        {
            viros_putchar(key);
        }

        // Backspace
        if (key == 0x08 && i >= 1)
        {
            out[i - 1] = 0x00;
            // -2 because we will +1 when we go to the continue
            i -= 2;
            continue;
        }

        out[i] = key;
    }

    // Add the null terminator
    out[i] = 0x00;
}

// Run a command
int viros_system_run(const char *command)
{
    char buf[1024];
    strncpy(buf, command, sizeof(buf));
    struct command_argument *root_command_argument = viros_parse_command(buf, sizeof(buf));
    if (!root_command_argument)
    {
        return -1;
    }

    return viros_system(root_command_argument);
}
