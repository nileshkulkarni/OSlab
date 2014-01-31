#ifndef _HEADER_H
#define _HEADER_H

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

#define MAXLINE 1000
#define DEBUG 0


#define CD "cd"
#define RUN "run"
#define PARALLEL "parallel"
#define CRON "cron"
#define EXIT "exit"
int run(char **tokens);
int execute(char** tokens);
int cd(char** tokens);
int otherCommands(char **tokens);
void SIGINT_handler(int sig);
void SIGUSR1_handler(int sig);



int biggestParent;
pid_t parent_ID;
pid_t child_process_ID;

typedef
struct {
    char** tokens;
    int nTokens;
} command;


command  parse(char *comm);
void pipedExec(command commands);


struct cronTask{
    char** argv;
    int min ;
    int hour ;
    int dayMon; 
    int month; 
    int dayOfWeek; 
};
struct cronTask *CTasks; 
int noOfCronTasks;

#endif
