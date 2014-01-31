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
      
	
	  ctokens = tokenize(cmd);
	  
	  if(strcmp(ctokens[0],CD) == 0){
			
            cd(ctokens);
	  }
    
	  else if(strcmp(ctokens[0],RUN) == 0){
		    child_process_ID = fork();
			if (child_process_ID == -1) {
				perror("fork failed");
			}
			else if(child_process_ID==0){
                biggestParent =0;
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
			child_process_ID = fork();
			if (child_process_ID == -1) {
				perror("fork failed");
			}
			else if(child_process_ID==0){
                biggestParent =0;
				//parent_ID = par; 
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














