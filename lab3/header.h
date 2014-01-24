// COMMANDS
//
#ifndef HEADER_H
#define HEADER_H

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#define MAXLINE 1000
#define DEBUG 1

#define CD "cd"
#define RUN "run"



pid_t child_process_ID;

int execute(char** tokens);
int cd(char** tokens);
int otherCommands(char **tokens);
#endif
