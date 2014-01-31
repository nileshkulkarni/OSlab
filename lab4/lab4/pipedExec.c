#include "header.h"
#include <stdio.h>
#include<sys/types.h>
#include <sys/wait.h>
void pipedExec(struct command* commands,int noOfCommands){
    int i=0;
    int inPipe ;
    int outPipe;
    int  
    int *pipefd =malloc(sizeof(int)*2);     
    pipe(pipefd)
    for(i=0;i<noOfCommands-1;i++){
                   
        if (pipe(pipefd)) {
            perror("pipe");
	        exit(127);
        
        }
        inPipe = pipefd[0];
        outPipe = pipefd[1];
        
        child_process_ID = fork();
        switch(child_process_ID){
            case -1:
                printf("fork error");
                exit(1);
                break;
            case 0:
                if(i ==0){
                    close(inPipe);
                    dup2(outPipe,1);
                    close(outPipe);
                }
                else{
                    

                }
            default:
                continue
        }
        
        close(outPipe);
        dup2(inPipe,0);
        close(inPipe);


    }
    while(commands[i] != NULL){
        int *pipefd =malloc(sizeof(int)*2);     
                   
        if (pipe(pipefd)) {
            perror("pipe");
	        exit(127);
        
        }
        child_process_ID = fork();
        switch(child_process_ID){
            case -1:
                printf("fork error");
                exit(1);
                break;
            case 0:
                close(inPipe);

        }

    }

    
}
