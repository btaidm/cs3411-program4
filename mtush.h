/**
 * Auther: Tim Bradt
 * File: mtush.h
 * The header file for mtush
 */
#ifndef MTUSH_H
#define MTUSH_H


/*
 * Name         : parse
 *
 * Synopsis     : int parse( char * command, char** out_args) 
 * Arguments	: command - the pipe string
 *				: out_args - the parsed commands
 * Description  : Parses user input of a pipe stream into seperate commands
 * 
 * Returns      : int - the number of commands
 */
int parse( char * command, char** out_args);

/*
 * Name         : parseSingle
 *
 * Synopsis     : int parseSingle( char* command, char** out_args)
 * Arguments    : command - A Single Command
 *				: out_args - The array of arguments
 * Description  : Parsed a single command into an array of strings
 * 
 * Returns      : int - the number of arguments
 */
int parseSingle( char* command, char** out_args);

/*
 * Name         : runCmd
 *
 * Synopsis     : int runCmd( char** args,  int argc)
 * Arguments    : args - The command and arguments
 *				: argc - the number of commands
 *				  
 * Description  : runs a single command
 * 
 * Returns      : int - the return value of the command
 */
int runCmd( char** args,  int argc);


/*
 * Name         : proccess
 *
 * Synopsis     : int proccess( char** cmds,  int argc)
 * Arguments    : cmds - the pipe stream
 *				: argc - the number of commands
 *
 * Description  : Processes a pipe stream
 * 
 * Returns      : int - 0
 */
int proccess( char** cmds,  int argc);

/*
 * Name         : cd
 *
 * Synopsis     : void cd( char** cmd,  int argc) *
 * Arguments    : cmd - the cd command
 * 				: argc - the number of arguments
 *
 * Description  : Changes Directory
 * 
 */
void cd( char** cmd,  int argc);

/*
 * Name         : setupPipes
 *
 * Synopsis     : pPipes(int* reading, int* writing, int args) *
 * Arguments    : int * reading - the array for the fds for reading portion of pipe
 *              : int * writing - the array for the fds for writing portion of pipe
 *              : int  args - the number of pipes to create
 *
 * Description  : Creates the pipes for the pipe stream
 * 
 * Returns      : -1 if pipe creation fails, 0 if successful
 */
int setupPipes(int* reading, int* writing, int args);

/*
 * Name         : runCmds
 *
 * Synopsis     : runCmds(char** cmds, const int argc) *
 * Arguments    : cmds - the commands to run
 *				: const int  argc - The number of cmds in pipe stream
 *
 * Description  : Runs the commands in a pipe stream allowing for proper piping
 * 
 * Returns      : 0
 */
int runCmds(char** cmds, const int argc);


/*
 * Name         : cleanPipes
 *
 * Synopsis     : int cleanPipes(int * reading, int* writing, int args) *
 * Arguments    : int * reading - the array for the fds for reading portion of pipe
 *				: int * writing - the array for the fds for writing portion of pipe
 *				: int  args - the number of pipes to create
 *
 * Description  : Cleans up the pipes after use
 * 
 * Returns      : int 
 */
int cleanPipes(int * reading, int* writing, int args);

#endif