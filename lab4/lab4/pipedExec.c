#include "header.h"
#include <stdio.h>
#include<sys/types.h>
#include <sys/wait.h>


void pipedExec(command commands){
  
    int noOfCommands = commands.nTokens;
    int i=0;
    int inPipe ;
    int outPipe;
    int prevOutpipe;
    int j=0; 
    int *pipefd =malloc(sizeof(int)*2);     
    if(commands.nTokens ==1){
        execute(tokenize(commands.tokens[0]));
        return; 
    }
    for(i=0,j=0;i<noOfCommands-1;i++,j++){

        if(strcmp(commands.tokens[j],"|") !=0){
           printf("Error in command \n");
            return; 

        }
        if (pipe(pipefd)) {
            perror("pipe");
	        exit(127);
        
        }
        inPipe = pipefd[0];
        outPipe = pipefd[1];
        
        child_process_ID = fork();
        if(child_process_ID ==-1){
                printf("fork error");
                exit(1);
                break;
        }
        else if(child_process_ID ==0){
                if(i ==0){
                    close(inPipe);
                    dup2(outPipe,1);
                    execute3(tokenize(commands.tokens[i]));
                    break;
                }
                else{
                    close(inPipe);
                    dup2(prevOutpipe,0);
                    dup2(outPipe,1); 
                    execute3(tokenize(commands.tokens[i]));
                    break;
                }
        } 
        else{
                prevOutpipe = inPipe;
                continue;
        }
        
    }
    dup2(prevOutpipe,0);
    close(inPipe);
    execute3(tokenize(commands.tokens[i]));
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
