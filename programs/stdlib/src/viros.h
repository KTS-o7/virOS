#ifndef viros_H
#define viros_H

#include <stddef.h>
#include <stdbool.h>

struct command_argument
{
    char argument[512];
    struct command_argument *next;
};

struct process_arguments
{
    int argc;
    char **argv;
};

void print(const char *filename);
int viros_getkey();

void *viros_malloc(size_t size);
void viros_free(void *ptr);
void viros_putchar(char c);
int viros_getkeyblock();
void viros_terminal_readline(char *out, int max, bool output_while_typing);
void viros_process_load_start(const char *filename);
struct command_argument *viros_parse_command(const char *command, int max);
void viros_process_get_arguments(struct process_arguments *arguments);

int viros_system(struct command_argument *arguments);
int viros_system_run(const char *command);
void viros_exit();

#endif
