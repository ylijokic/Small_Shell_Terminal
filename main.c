/*
    This file contains the main entry function of the program. This file utilizes the
    Command.h file where the Command struct is defined, as well as the helper_functions.h file and the built_in_commands.h file.

    There function in this file:
        1.) handleSigINT()
        2.) handleSigSTP()
        3.) getCommand()
        4.) main()
*/

#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "Command.h"
#include "helper_functions.h"
#include "built_in_commands.h"


int noBackgroundMode = 0;
int arg_flag;
int childExitStatus;

/*  handleSigINT()
    Functionality: 
            This function handles the ^C command when entered by the user. 
    Parameters:
            1.) signal int sent to command
    Returns:
            Does not return anything. Prints out the termintaed signal.
*/
void handleSigINT(int sig)
{
        char *message = "\nterminated by signal \n";
        write(1, message, 23);
        fflush(stdout);

        // getStatus(childExitStatus);
}

/*  handleSigSTP()
    Functionality: 
            This function handles the ^Z command when entered by the user. 
    Parameters:
            1.) signal int sent to command
    Returns:
            Does not return anything. Prints out whether the shell is entering or exiting foreground 
            only mode. In this mode the & flag is ignored, and all commands are run in the foreground.
*/
void handleSigSTP(int sig)
{
        // Trigger the arg_flag so that the command prompt ignores the & input
        arg_flag = 1;
        if (noBackgroundMode == 0)
        {
                char *message = "\nEntering foreground-only mode (& is now ignored)\n";
                write(1, message, 51);
                fflush(stdout);
                noBackgroundMode = 1;
        }
        else 
        {
                char *message = "\nExiting foreground-only mode\n";
                write(1, message, 31);
                fflush(stdout);
                noBackgroundMode = 0;
        }
}

/*  getCommand()
    Functionality: 
             This function gives the shell command prompt to the user and then processes the input using the strtok 
             function. Depending on what the input from the user, the function assigns values to the data members of the 
             Command struct that is created. It also sets necessary flags for input/output and background processes.
    Parameters:
            1.) process ID of the parent process
    Returns:
            returns the newly created Command struct that holds all the data members associated with the command, 
            command arguments, flags, and input/output files. 
*/
struct Command *getCommand(char *process_id)
{
        // char array to hold the user input
        char input[2048];
        int size = 512;

        // Expansion variable substring
        char *var = "$$";

        // current toke, previous token, and seperators to distinguish tokens.
        char *cmd_token;
        char *prev_cmd_token = "";
        const char separator[10] = " \t\n\0";

        // Allocate memory for a new Command struct
        struct Command *new_command = (struct Command *)malloc(sizeof(struct Command));

        // Flags to determine if a token is a valid argument
        int i = 0;
        int arg = 0;
        arg_flag = 0;

        // Print smallsh prompt
        printf(":: ");
        fflush(stdout);

        // Use fgets() to read input from stdin
        fgets(input, size, stdin);

        // Return empty Command struct if first char is '#'
        if (strncmp(input, "#", 1) == 0)
        {
                new_command->cmd_args[arg] = "#";
                return new_command;
        }

        // Call replaceString from helper_functions.h
        // The function will search user input for the expansion variable and replace it with process ID
        replaceString(input, 2048, var, process_id);

        // Use strtok to find first space-seperated token of input
        cmd_token = strtok(input, separator);
        while (cmd_token != NULL)
        {
                if (i == 0)
                {
                        // Check for Comment or NULL
                        if (strcmp(cmd_token, "#") == 0 || cmd_token == NULL)
                        {
                                break;
                        }
                }

                // Check for Input File Name by comparing previous token to "<"
                if (strcmp(prev_cmd_token, "<") == 0)
                {
                        // set Command struct input_file data member 
                        new_command->input_file = strdup(cmd_token);
                        arg_flag = 1;
                }

                // Check for Output File Name by comparing previous token to ">"
                if (strcmp(prev_cmd_token, ">") == 0)
                {
                        // set Command struct output_file data member
                        new_command->output_file = strdup(cmd_token);
                        arg_flag = 1;
                }

                // If the current token is not an input/out file or the & flag then add it to the cmd_args array
                if (arg_flag == 0 && strcmp(cmd_token, "<") != 0 && strcmp(cmd_token, ">") != 0 && strcmp(cmd_token, "&") != 0)
                {
                        // cmd_args[0] should store the actual command while the other values should be the arguments
                        new_command->cmd_args[arg] = strdup(cmd_token);
                        arg++;
                }

                // Keep track of what number token strtok is on and also the previous token
                i++;
                prev_cmd_token = cmd_token;
                cmd_token = strtok(NULL, separator);
        }
        // Set last argument to NULL
        new_command->cmd_args[arg] = NULL;
        // Once at the end of the user input, check Flag for Running in Background
        if (strcmp(prev_cmd_token, "&") == 0 && noBackgroundMode == 0)
        {
                new_command->background = 1;
        }
        else
        {
                new_command->background = 0;
        }

        // Return the newly created Command struct
        return new_command;
}

/*
    Functionality: 
            Main function for program. Forks the process id of the shell, and depending on if it is a child or
            parent process, the function either calls a built in command or calls exec function.
    Parameters:
            N/A
    Returns:
            Return exit status
    Sources Cited:
            SIG Handling: https://canvas.oregonstate.edu/courses/1784217/pages/exploration-signal-handling-api?module_item_id=19893105
            SIG Handling https://c-for-dummies.com/blog/?p=3457
            Background processes: https://repl.it/@cs344/42waitpidnohangc
*/
int main()
{
        // Process ID for the command shell
        pid_t process_id;
        process_id = getpid();
        // Store as a string
        char process[32];
        sprintf(process, "%d", process_id);

        // Child Process ID
        pid_t spawnid;

        // SIGINT sigaction struct with default sa_handler and no flags
        struct sigaction SIGINT_action = {{0}};
        SIGINT_action.sa_handler = SIG_IGN;
        sigfillset(&(SIGINT_action.sa_mask));
        SIGINT_action.sa_flags = 0;
        // Cal sigaction function
        sigaction(SIGINT, &SIGINT_action, NULL);

        // SIGSTP sigaction struct with sa_handler set to handleSigSTP function
        struct sigaction SIGSTP_action = {{0}};
        SIGSTP_action.sa_handler = handleSigSTP;
        sigfillset(&SIGSTP_action.sa_mask);
        SIGSTP_action.sa_flags = 0;
        // Call sigaction function
        sigaction(SIGTSTP, &SIGSTP_action, NULL);


        int status = 1;
        while (status)
        {
                // Create new Command struct with getCommand function
                struct Command *p = getCommand(process);

                // Check for Null command
                if (p->cmd_args[0] == NULL || strcmp(p->cmd_args[0], "#") == 0)
                {
                        continue;
                }
                // Check for exit built in command
                if(strcmp(p->cmd_args[0], "exit") == 0)
                {
                        exitShell();
                }
                // Check for cd built in command
                else if(strcmp(p->cmd_args[0], "cd") == 0)
                {
                        changeDirectory(p->cmd_args);
                }
                // Check for status built in command
                else if(strcmp(p->cmd_args[0], "status") == 0)
                {
                        p->background = 0;
                        getStatus(childExitStatus);
                }
                // Otherwise the command is not a built in 
                else
                {
                        // use fork() to create the child process
                        spawnid = fork();

                        // Switch statement to determine what to do with child
                        switch (spawnid)
                        {
                        // Error on fork
                        case -1:
                                perror("fork() failed!");
                                exit(1);
                                break;
                        // current process is child
                        case 0:
                                // Handle SIGINT for background process
                                if (p->background != 1)
                                {
                                        SIGINT_action.sa_handler = handleSigINT;
                                }
                                // Open input file if one was specified
                                if (p->input_file != NULL)
                                {
                                        int inputFile = open(p->input_file, O_RDONLY);
                                        if (inputFile == -1)
                                        {
                                                perror("input open()");
                                                fflush(stdout);
                                                break;
                                        }

                                        // Create copy of file descriptor using dup2()
                                        int result = dup2(inputFile, 0);
                                        if (result == -1)
                                        {
                                                perror("input dup2()");
                                                fflush(stdout);
                                                exit(2);
                                        }
                                        close(inputFile);
                                }

                                // Handle output file if one was specified
                                if (p->output_file != NULL)
                                {
                                        int outputFile = open(p->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                                        if (outputFile == -1)
                                        {
                                                perror("output open()");
                                                fflush(stdout);
                                                exit(1);
                                        }

                                         // Create copy of file descriptor using dup2()
                                        int result = dup2(outputFile, 1);
                                        if (result == -1)
                                        {
                                                perror("output dup2()");
                                                fflush(stdout);
                                                exit(2);
                                        }
                                        close(outputFile);
                                }

                                // set SIGINT sa_handler to handleSigINT function
                                SIGINT_action.sa_handler = handleSigINT;

                                // Run command using the execvp function. Passing p->cmd_args as parameters.
                                // p._cmd_args[0] stores command name and other indices store arguments
                                if(execvp(p->cmd_args[0], p->cmd_args) < 0)
                                {
                                        // Print error if execvp returns -1
                                        perror("execve");
                                        exit(2);
                                }
                                
                                break;
                        // Default action for if process is a parent process
                        default:
                                if (p->background == 1)
                                {
                                         // Used for when background flag is set
                                        printf("background pid is %d\n", spawnid);
                                        fflush(stdout);
                                }
                                else 
                                {
                                        // Otherwise run as foreground and wait for process to complete
                                        spawnid = waitpid(spawnid, &childExitStatus, 0);
                                }
                              
                                break;
                        }
                
                }
                // Run process in background by calling waitpid with WNOWANG flag
                spawnid = waitpid(-1, &childExitStatus, WNOHANG);

                // If there is another background process then display message and end it.
                if (spawnid > 0)
                {
                        printf("background process %d is done: ", spawnid);
                        fflush(stdout);
                        getStatus(childExitStatus);
                        spawnid = waitpid(-1, &childExitStatus, WNOHANG);
                }

                // Free memory allocated for Command struct
                freeCommandMemory(p);
                    
        }
        exit(0);
}