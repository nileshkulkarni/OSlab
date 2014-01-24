#ifndef _HEADER_H
#define _HEADER_H

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

#define MAXLINE 1000
#define DEBUG 1


#define CD "cd"
#define RUN "run"
#define PARALLEL "parallel"

int run(char **tokens);
int execute(char** tokens);
int cd(char** tokens);
int otherCommands(char **tokens);
void SIGINT_handler(int sig);
void SIGUSR1_handler(int sig);


int biggestParent;
pid_t parent_ID;
pid_t child_process_ID;

#endif
