/*
    This file contains the built in commands exit, cd, and status. The functions run the commands in the foreground 
    and ignore the & flag if passed in. 

    There are five functions in this file:
        1.) exitShell()
        2.) changeDirectory()
        3.) getStatus()
*/

#ifndef BUILT_IN_COMMANDS_H
#define BUILT_IN_COMMANDS_H

#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "Command.h"

/*  exitShell()
    Functionality: 
            This function exits the command shell and ends all running processes. 
    Parameters:
            N/A
    Returns:
            Does not return anything. Exits the command Shell
*/
void exitShell()
{
    exit(0);
}


/*  changeDirectory()
    Functionality: 
            This function changes directory using the chdir() function. If a directory is specified at the cmd_args[1] index
            then shell changes to that directory, otherwise the HOME directory is selected. 
    Parameters:
            1.) cmd_args char array that holds the command(cd) at index [0] and the sirectory name held at index [1]
    Returns:
            Does not return anything. Calls the chdir() function to change the directory
    Sources Cited:
            https://www.geeksforgeeks.org/chdir-in-c-language-with-examples/
*/
void changeDirectory(char *cmd_args[512])
{
    if (cmd_args[1] != NULL)
    {
        chdir(cmd_args[1]);
        // perror("No Such File or Directory\n");
    }
    else
    {
        char dir[1024];
        getcwd(dir, 1024);
        chdir(getenv("HOME"));
    }
}

/*  getStatus()
    Functionality: 
            This function takes in the child process exit status and uses the WIFEXITSTATUS function to  determine what the exit status is. 
    Parameters:
            1.) The child process exit status
    Returns:
            Does not return anything. Prints the exit status of the command shell.
    Sources Cited:
            https://repl.it/@cs344/42waitpidexitc#main.c
            https://www.geeksforgeeks.org/exit-status-child-process-linux/
*/
void getStatus(int status)
{
    // Returns true if child terminated normally
    if (WIFEXITED(status))
    {
        // Print exit status of child
        printf("exit value %d\n", WEXITSTATUS(status));
        fflush(stdout);
    }
    // Otherwise child was not terminated normally
    else
    {
        printf("terminated by signal %d\n", status);
        fflush(stdout);
    }
}

#endif