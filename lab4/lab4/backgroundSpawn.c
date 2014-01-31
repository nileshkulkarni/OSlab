#include "header.h"

extern pid_t backgroundGID;



void backgroundSpawn(char *input){
	
	
	int h = strlen(input) - 1;
	
	while(input[h] != '&'){
		h--;
	}
	
	input[h] = '\0';
	
	pid_t bgpi = fork();   //background process id
	
	//printf("Over here : \n");
	
	
	if(bgpi == -1){
		perror("Fork Error\n");
		return;
	}
	
	
	if(bgpi == 0){	
		jashExec(input);
		exit(0);
	}
	
	
	else{
		
//		printf("bgpi is %d \n" , bgpi);
		setpgid(bgpi , backgroundGID);
	}
}


