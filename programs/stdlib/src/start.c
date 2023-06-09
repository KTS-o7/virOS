/*
    ? c_start:
    * This code is the entry point for a program running on the Hardik Hyperion OS.
    * The function retrieves the arguments passed to the program
    * and then calls the main function.
*/

#include "viros.h"

extern int main(int argc, char **argv);

// This is the entry point for the program
void c_start()
{
    struct process_arguments arguments;
    viros_process_get_arguments(&arguments);

    int res = main(arguments.argc, arguments.argv);
    if (res == 0)
    {
    }
}
