/*
    This file contains helper functions to assist with finding/replacing substrings and 
    freeing up memory.

    There are five functions in this file:
        1.) replaceString()
        2.) freeCommandMemory()
*/

#ifndef HELPER_FUNCTIONS
#define HELPER_FUNCTIONS

#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "Command.h"

/*  replaceString()
    Functionality: 
            This is a helper function used to search a string for a substring, and replace that substring with a different one. 
            The function is specifically used to search the user command line input for the '$$' variable and replace it with the process ID
            of the parent process. 
    Parameters:
            1.) source string (string to search for substring)
            2.) size of source string
            3.) substring to search for
            4.) substring to replace with
    Returns:
            Returns a new string which contains the user input that has replaced the $$ expansion variable with the parent process ID.
    Cited Sources:
            I utilized thw following resource to help with the implementation of this function. The youtube vidoe is an explanation of the source code link.
            https://pastebin.com/srLi6QZ1
            https://www.youtube.com/watch?v=0qSU0nxIZiE&t=631s
*/
char *replaceString(char *source, size_t size, char *substr, char *replace)
{
    char *substring = strstr(source, substr);
    if (substring == NULL)
    {
        return NULL;
    }

    // Make sure size of source string is larger than what you need to replace it with
    if (size < strlen(source) + strlen(replace) - strlen(substr) + 1)
    {
        printf("Size is too small!\n");
        fflush(stdout);
        return NULL;
    }

    // Utilize memove to allocate memory for replacment substring
    memmove(
        substring + strlen(replace),
        substring + strlen(substr),
        strlen(substring) - strlen(substr) + 1);

    memcpy(substring, replace, strlen(replace));
    return substring + strlen(replace);
}

/*  freeCommandMemory()
    Functionality: 
            This function sets the values of all arguments, input file, and output file to NULL. It also frees the memory
            that was allocated for the Command struct.
    Parameters:
            1.) Pointer to Command struct
    Returns:
            Does not return anything. Frees for struct and sets all arguments to NULL.
*/
void freeCommandMemory(struct Command *p)
{
    for (int i = 0; p->cmd_args[i] != NULL; i++)
    {
        p->cmd_args[i] = NULL;
    }

    if (p->input_file != NULL)
    {
        p->input_file = NULL;
    }

    if (p->output_file != NULL)
    {
        p->output_file = NULL;
    }

    // Free memory set aside for Command struct
    free(p);
}

#endif