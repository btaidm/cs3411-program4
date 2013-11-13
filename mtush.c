/**
 * Auther: Tim Bradt
 * File: mtush.c
 * The C file fot mtush.
 *
 * /////////////////////////////////////////////////////////
 * /////////////////////////////////////////////////////////
 * ////// FUNCTION DESCRITIONS ARE IN THE HEADER FILE //////
 * /////////////////////////////////////////////////////////
 * /////////////////////////////////////////////////////////
 */
 
#include "mtush.h"
#include <stdio.h>
#include "colors.h"
#include "string.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>

// Storage for commands and prompt
char promt[1024];
char* commands[1024];
char* cmd[256];

int main(void)
{
	while(1)
	{
		// Create prompt string
		sprintf(promt, "mtush " GRN "%s" NRM "> ", getcwd(NULL, 0));
		
		// Get user input
		char* input = readline(promt);
		if(!input)
			printf("Error reading user input.\n");
		else
		{
			// Check for exit or quit commands
			if(strcmp(input, "exit") == 0 ||
			   strcmp(input, "quit") == 0)
			{
				free(input);
				exit(0);
			}
			// Add to history
			add_history(input);
			
			// Parse the pipe sequence into sepreate commands
			int pipes = parse((char*)input,commands);
			
			// Process the pipe stream
			proccess(commands, pipes);
			free(input);
		}
	}
}

int parse(char * command, char** out_args)
{
	// Copy the pointer
	char * cpy = command;
	char* pch;
	
	// Start tokenizing
	pch = strtok(cpy,"|");
	int count = 0;
	
	while(pch != NULL)
	{
		out_args[count] = pch;
		count++;
		pch = strtok(NULL, "|");
	}
	// NULL end the arguments
	out_args[count] = NULL;
	return count;
}

int parseSingle(char* command, char** out_args)
{
	// Copy the pointer
	char * cpy = command;
	char* pch;
	
	// Start tokenizing
	pch = strtok(cpy," ");
	int count = 0;
	
	while(pch != NULL)
	{
		out_args[count] = pch;
		count++;
		pch = strtok(NULL, " ");
	}
	// NULL end the arguments
	out_args[count] = NULL;
	return count;
}

int proccess(char** cmds, const int argc)
{
	// Check to see if single command
	if(argc == 1)
	{
		int args = parseSingle(cmds[0], (char**)cmd);
		
		// Check for cd
		if(strcmp(cmd[0], "cd") == 0)
		{
			// Run cd and return
			cd((char**)cmd, args);
			return 0;
		}
		
		// run the command
		int ret = runCmd((char**)cmd, args);
		if(ret)
		{
			perror((char*)cmd[0]);
		}
		
		// and return
		return 0;
	}
	
	// If all that is skipped, then it is a multiple pipe stream
	// Run the stream
	runCmds(cmds,argc);
	
	return 0;
}

int runCmds(char** cmds, const int argc)
{
	// Create buffers for holding pipe fds
	// Pipe and Forking code based on this stackoverflow question: 
	// http://stackoverflow.com/questions/1694706/problem-with-piping-commands-in-c
	
	int* reading = malloc(argc * sizeof(int));
	int* writing = malloc(argc * sizeof(int));
	
	// Sets up the pipes
	if(setupPipes(reading,writing, argc))
	{
		// If it failed for anyreason clean up and return
		cleanPipes(reading,writing, argc);
		free(reading);
		free(writing);
		return 0;
	}
	
	// Fork for each command
	for(int i = 0; i < argc; i++)
	{
		parseSingle(cmds[i], (char**)cmd);
		
		pid_t childpid;
		int status;
		childpid = fork();
		
		if(childpid >= 0)
		{
		
			if(childpid == 0)
			{
				// Child process
				
				// Map to standard out
				if(writing[i] != -1)
				{
					close(1);
					dup2(writing[i],1);
				}

				// Map to standard in
				if(reading[i] != -1)
				{
					close(0);
					dup2(reading[i],0);
				}

				// Run cmd
				execvp(cmd[0],cmd);
				
				// Print error if it falls through and exit child process 
				//perror(cmd[0]);
				fprintf(stderr, "mtush: command not found: %s\n", cmd[0]);
				exit(-1);
			}
			else 
			{
				// wait for child to finish
				wait(&status);
				
				// Close the pipes
				close(writing[i]);

				if(i > 0) 
				{
					close(reading[i]);
				}
				if(status)
					break;
			}
		}
		else
		{
			// If fork fails clean up and exit
			cleanPipes(reading,writing, argc);
			free(reading);
			free(writing);
			return 0;
		}
	}
	
	// Clean up and exit
	cleanPipes(reading,writing, argc);
	free(reading);
	free(writing);
	
	return 0;
}

int setupPipes(int* reading, int* writing, int args)
{
	// Set everything to -1
	memset(reading, 0xff, args * sizeof(int));
	memset(writing, 0xff, args * sizeof(int));
	int ret  = 0;
	// Start creating the pipes
	for(int arg = 0; arg < args-1; arg++)
	{
		int fileds[2];
		if(pipe(fileds))
		{
			// Failure return -1
			perror("pipe");
			ret = -1;
			break;
		}
		reading[arg+1] = fileds[0];
		writing[arg] = fileds[1];
	}
	return ret;
}

int cleanPipes(int * reading, int* writing, int args)
{
	// Loop through it make sure pipes are closed
	for(int arg = 0; arg < args-1; arg++)
	{
		close(reading[arg+1]);
		close(writing[arg]);
	}
	return 0;
}


int runCmd( char** args,  int argc)
{
	/* for(int i = 0; i < argc; i++)
	{
		printf("%s ",args[i]);
	}
	printf("\n"); */
	int ret = 0;
	// fork for running command
	if(argc > 0)
	{
		pid_t pid = fork();
		
		if(pid >= 0)
		{
			// Check if child
			if(pid ==0)
			{
				// Child
				// Run command
				execvp(args[0],args);
				fprintf(stderr, "mtush: command not found: %s\n", cmd[0]);
				exit(0);
			}
			else
			{
				// Parent
				//Get return of command
				wait(&ret);
			}
		}
		else
		{
			// Fork Error
			perror("fork");
			ret = 0;
		}
	}
	return ret;
}

void cd( char** cmd,  int argc)
{
	// If single cd
	if(argc == 1)
	{
		//Go Home
		char* home = getenv("HOME");
		if(home != NULL)
		{
			if(chdir(home))
			{
				perror("cd failed");
			}
			
		}
		return;
	}
	
	// Else change directory printing error if problem
	if(chdir(cmd[1]))
	{
		perror(cmd[1]);
	}
}