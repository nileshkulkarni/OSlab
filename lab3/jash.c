#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>	
#include "header.h"

#define _POSIX_SOURCE

#include <unistd.h>




//declarations
char ** tokenize(char*);
extern pid_t parent_ID;
extern int biggestParent;
extern struct cronTask *CTasks; 
extern int noOfCronTasks; 
pid_t parallelGID;
int parallelRunning;

int main(int argc, char** argv){

	//Setting the signal interrupt to its default function. 
	if(signal(SIGINT, SIGINT_handler) == SIG_ERR){
        printf("SIGINT install error\n");
        exit(1);
    }

	if(signal(SIGUSR1, SIGUSR1_handler) == SIG_ERR){
        printf("SIGINT install error\n");
        exit(1);
    }
	//nAllocating space to store the previous commands.
	int numCmds = 0;
	char **cmds = (char **)malloc(1000 * sizeof(char *));
	parallelRunning = 0;
	int printDollar = 1;
    biggestParent = 1;
    parallelGID = 100;
    CTasks = malloc(sizeof(struct cronTask) *1000);
	char input[MAXLINE];
	char** tokens;

	int notEOF = 1;
	int i;

	FILE* stream = stdin;
    parent_ID = getpid();
	child_process_ID = -1;
   while(notEOF) { 
		if (printDollar == 1){ 
			printf("$ "); // the prompt
			fflush(stdin);
		}
      
		char *in = fgets(input, MAXLINE, stream); //taking input one line at a time

		//Checking for EOF
/*		if (in == NULL){
			if (DEBUG) printf("EOF found\n");
			exit(0);
			
		}
*/
		//add the command to the command list.
		cmds[numCmds] = (char *)malloc(sizeof(input));
		strcpy(cmds[numCmds++], input); 

		// Calling the tokenizer function on the input line    
		
		
		tokens = tokenize(input);
		
			
		// Uncomment to print tokens
/*		
        for(i=0;tokens[i]!=NULL;i++){
			printf("%s\n", tokens[i]);
		}
*/		
		

		if(tokens[0] == NULL)
            continue;
            
            
        else{ 
            if(strcmp(tokens[0] , PARALLEL) == 0){
				parallel(input+strlen(PARALLEL));
				int *status = malloc (sizeof(int));
				if(parallelRunning){
					waitpid(-parallelGID,status,0);
					parallelGID = 1000;
					parallelRunning=0;
				}
			}
			else if(strcmp(tokens[0] , EXIT) == 0){
				if(child_process_ID!=-1){
					kill(child_process_ID,9);
				}
				kill(0,9);
			}
		
			else if(strcmp(tokens[0] , CRON) == 0){
				read_cron_file(tokens[1]);
			}
			else        
				execute(tokens);
		}
		
		
		
	}
  
  
//	printf("Print and deallocate %s\n", tokens[0]);
	// Freeing the allocated memory	
	for(i=0;tokens[i]!=NULL;i++){
		free(tokens[i]);
	}
	free(tokens);
	return 0;
}

/*the tokenizer function takes a string of chars and forms tokens out of it*/
char ** tokenize(char* input){
	int i;
	int doubleQuotes = 0;
	
	char *token = (char *)malloc(1000*sizeof(char));
	int tokenIndex = 0;

	char **tokens;
	tokens = (char **) malloc(MAXLINE*sizeof(char**));
 
	int tokenNo = 0;
	
	for(i =0; i < strlen(input); i++){
		char readChar = input[i];
		
		if (readChar == '"'){
			doubleQuotes = (doubleQuotes + 1) % 2;
			if (doubleQuotes == 0){
				token[tokenIndex] = '\0';
				if (tokenIndex != 0){
					tokens[tokenNo] = (char*)malloc(sizeof(token));
					strcpy(tokens[tokenNo++], token);
					tokenIndex = 0; 
				}
			}
		}
		else if (doubleQuotes == 1){
			token[tokenIndex++] = readChar;
		}
		else if (readChar == ' ' || readChar == '\n' || readChar == '\t'){
			token[tokenIndex] = '\0';
			if (tokenIndex != 0){
				tokens[tokenNo] = (char*)malloc(sizeof(token));
				strcpy(tokens[tokenNo++], token);
				tokenIndex = 0; 
			}
		}
		else{
			token[tokenIndex++] = readChar;
		}
	}
	
	if (doubleQuotes == 1){
		token[tokenIndex] = '\0';
		if (tokenIndex != 0){
			tokens[tokenNo] = (char*)malloc(sizeof(token));
			strcpy(tokens[tokenNo++], token);
		}
	}
	
	return tokens;
}
