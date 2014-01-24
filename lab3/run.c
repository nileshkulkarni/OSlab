#include <string.h>
#include <assert.h>
#include "header.h"

int run(char **tokens){
	
	assert(strcmp(tokens[0] , "run") == 0);
	
	FILE *fp = fopen(tokens[1],"r"); // read mode
	
	char cmd[MAXLINE];
	if( fp == NULL )
   {
      printf("Could not find the batch file %s \n" , tokens[1]);
      return;
   }
   
   
   char **ctokens;
    while(fgets (cmd, MAXLINE, fp) != NULL){
      
  //    printf("command Name %s \n",cmd);
	
	  ctokens = tokenize(cmd);
//	  printf("%s : command \n" , ctokens[0]);
	  
	  if(strcmp(ctokens[0],CD) == 0){
			
            cd(ctokens);
	  }
    
	  else if(strcmp(ctokens[0],RUN) == 0){
			pid_t ccpid = fork();
			if (ccpid == -1) {
				perror("fork failed");
			}
			else if(ccpid==0){
				run(ctokens);
			}
			else{
				//  printf("forked\n");
			}
			int *stat = malloc(sizeof(int));
			wait(stat);
			free(stat);
	} 
      else{
			pid_t ccpid = fork();
			if (ccpid == -1) {
				perror("fork failed");
			}
			else if(ccpid==0){
				otherCommands(ctokens);
			}
			else{
				//  printf("forked\n");
			}
			int *stat = malloc(sizeof(int));
			wait(stat);
			free(stat);
      }
   }
       
      
}
