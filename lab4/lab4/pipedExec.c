#include "header.h"
#include <stdio.h>
#include<sys/types.h>
#include <sys/wait.h>





opIndex(char *ch){
	
	if(strcmp(ch , "<") == 0) return 0;
	if(strcmp(ch , ">") == 0) return 1;
	if(strcmp(ch , ">>") == 0) return 1;
	
}








void pipedExec(command commands){
  
    int noOfCommands = commands.nTokens;
    int i=0;
    int inPipe ;
    int outPipe;
    int prevOutpipe;
    int j=0; 
    int pipefd[2];     
    if(commands.nTokens ==1){
        execute(tokenize(commands.tokens[0]));
        return; 
    }
    for(i=0,j=0;i<noOfCommands-1;i=i+2,j++){
/*
        if(strcmp(commands.tokens[j],"|") !=0){
           printf("Error in command \n");
            return; 
		}
*/		

		printf(" now %s \n", commands.tokens[i]);

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
                if(i==0){
					biggestParent = 0;
                    close(inPipe);
                    printf(" 2comes here ::  \n");
                    dup2(outPipe,1);
                  //  printf(" 3comes here %d ::  \n");
                    close(outPipe);
                    //printf("comes here ::  \n");
                    execute3(tokenize(commands.tokens[i]));
					kill(0,9);
                }
                else{
          //          close(inPipe);
                 //   sleep(1);
                    dup2(prevOutpipe,0);
                    dup2(outPipe,1); 
                    execute3(tokenize(commands.tokens[i]));
                    break;
                }
        } 
        else{
                int s;
     //           wait(&s);
    //            kill(
                prevOutpipe = inPipe;
                continue;
        }
        
   }
 
if(biggestParent == 1){    
 //  	printf(" now %s \n", commands.tokens[i]);
 //   sleep(1);
    dup2(prevOutpipe,0);
 // printf("parent now %s \n", commands.tokens[i]);
	close(inPipe);
    execute3(tokenize(commands.tokens[i]));
  }  
}


void IORedirection(command commands){
	
//	printf("No of Tokens: %d \n  ",commands.nTokens);
	
    int i=0;
    int inPipe ;
    int outPipe;
    int prevOutpipe;
    
    
    int count[3];
    for(int h=0;h<3;h++)
		count[h] = 0;
    
    
    int j;
    for(j=0;j<commands.nTokens;j++){
			
			if((strcmp(commands.tokens[j],">")==0) ||(strcmp(commands.tokens[j],"<")==0) || (strcmp(commands.tokens[j],">>")==0)){
				count[opIndex(commands.tokens[j])]++;
				if(count[opIndex(commands.tokens[j])] > 1){
						printf("Error : more than 1 \"%s\" Redirections \n" , commands.tokens[j]);
						return;
				}	
			}
	}		
    
    
    
    
    
    int pipefd[2];   

    child_process_ID = fork();
    if(child_process_ID==0){
        for(j=0;j<commands.nTokens;j++){
			
	       if((strcmp(commands.tokens[j],">")==0)){

                j++;
                if(j<commands.nTokens){
                    commands.tokens[j][strlen(commands.tokens[j]) - 1] = '\0';
                    int newfd  = open(commands.tokens[j], O_CREAT|O_TRUNC|O_WRONLY, 0644);
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
            
            
          if((strcmp(commands.tokens[j],">>")==0)){

                j++;
                if(j<commands.nTokens){
                   commands.tokens[j][strlen(commands.tokens[j]) - 1] = '\0';
                   int newfd  = open(commands.tokens[j], O_CREAT|O_APPEND|O_WRONLY , 0644);
                    //int newfd = open(commands.tokens[j],'r');
                    dup2(newfd,1);
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
