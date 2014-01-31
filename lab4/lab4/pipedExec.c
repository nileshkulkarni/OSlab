#include "header.h"
#include <stdio.h>
#include<sys/types.h>
#include <sys/wait.h>

int checkJash(char* data){

    char** tokens = tokenize(data);
    int i=0;
    while(tokens[i]!=NULL){
        if((strcmp(tokens[i],CD)==0) || (strcmp(tokens[i],RUN)==0) ||(strcmp(tokens[i],CRON)==0)){
           return 1; 
        }
        i++;
    }
    return 0;
}
void pipedExec(command commands){
    int child1;
    int child2;
    int noOfCommands = commands.nTokens;
    int i=0;
    int inPipe ;
    int outPipe;
    int prevOutpipe;
    int j=0; 
    child_process_ID = fork();
    if(child_process_ID ==0){

        int pipefd[2];
        if(commands.nTokens != 3){
            printf("Wrong piping directive\n");
            return;     
        }
       
        if(checkJash(commands.tokens[0])){
            printf("Erroneous Command here, recieved a cd run or cron\n");
            return ;
        }
        if(checkJash(commands.tokens[2])){
            printf("Erroneous Command here, recieved a cd run or cron\n");
            return;
        }
        
        if(pipe(pipefd)){
            perror("pipe\n");
        }
        child_process_ID = fork();
         
        if(child_process_ID==-1){
            printf("Fork error");
        }
        else if(child_process_ID==0){
            //child process
            close(pipefd[0]);  /* the other side of the pipe */
            dup2(pipefd[1], 1);  /* automatically closes previous fd 1 */
            close(pipefd[1]);  /* cleanup */
            execute3(tokenize(commands.tokens[0]));  
            exit(0);
            return;
        }
        else{
            //inside parent 
            
                close(pipefd[1]);  /* the other side of the pipe */
                dup2(pipefd[0], 0);  /* automatically closes previous fd 1 */
                close(pipefd[0]);
                execute3(tokenize(commands.tokens[2]));  
                int st;
                wait(&st);
                exit(0);
                return;    

        }
    }
    else{
        int st;
        wait(&st);
        child_process_ID = -1;
    }
}


void IORedirection(command commands){
	
	printf("comes here : %d \n  ",commands.nTokens);
	
    int i=0;
    int inPipe ;
    int outPipe;
    int prevOutpipe;
    
    int *pipefd =malloc(sizeof(int)*2);    

    child_process_ID = fork();
    if(child_process_ID==0){
        int j=0;
        for(j=0;j<commands.nTokens;j++){
            if((strcmp(commands.tokens[j],">")==0)){

                j++;
                if(j<commands.nTokens){
                    
                    //int newfd = open("a.txt",'w');
                    
                    commands.tokens[j][strlen(commands.tokens[j]) - 1] = '\0';
                    
                    int newfd  = open(commands.tokens[j], O_CREAT|O_TRUNC|O_WRONLY, 0644);
                    printf("here ** %s **" , commands.tokens[j]);
                    dup2(newfd,1);
                }
            }
            
            if((strcmp(commands.tokens[j],"<")==0)){

                j++;
                if(j<commands.nTokens){
                   commands.tokens[j][strlen(commands.tokens[j]) - 1] = '\0';
                   int newfd  = open(commands.tokens[j], O_CREAT|O_TRUNC|O_WRONLY, 0644);
                    //int newfd = open(commands.tokens[j],'r');
                    dup2(newfd,0);
                }
            }
        }
        execute3(tokenize(commands.tokens[0]));
    }
    else{
       int *stat = malloc(sizeof(int));
       while(1){
           wait(stat);
           if(WIFEXITED(*stat)) break;
       } 
       free(stat);
       child_process_ID=-1;
    }
}
