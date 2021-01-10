/*
    This file contains the information for the Command Struct object.
    The Command struct contains all information related to individual commands, 
    as well ass command arguments, flags, and inpput/output files.

    Data Members:
        cmd_args (char array) ->    command and associated command arguments
        background: (int) ->        flag to keep track of if command is to be run in the background
        input_file: (string) ->     name of input_file 
        output_file: (string) ->    name of output file
*/

#ifndef COMMAND_H
#define COMMAND_H

struct Command
{
    /* data */
    char *cmd_args[512];

    char* input_file;
    char* output_file;
    int background;
};

#endif