#ifndef _HEADER_H
#define _HEADER_H

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>


#define MAXLINE 1000
#define DEBUG 1


#define CD "cd"
#define RUN "run"


int run(char **tokens);
int execute(char** tokens);
int cd(char** tokens);
int otherCommands(char **tokens);




pid_t child_process_ID;

#endif
