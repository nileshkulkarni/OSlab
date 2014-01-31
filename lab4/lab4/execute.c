#include "header.h"

extern pid_t child_process_ID;
extern int biggestParent;
extern int parallelRunning;
extern pid_t parallelGID;
int read_cron_file(char *file){
  child_process_ID= fork();
	
	char cmd[MAXLINE];
	char **tokens;
//    child_process_ID =0;	
	if(child_process_ID == 0){
		biggestParent = 0;
		FILE *fp = fopen(file,"r"); // read mode
		
		if( fp == NULL ){
			printf("Could not find the batch file %s \n" , file);
			return -1;
		}

		noOfCronTasks = 0;
    while(fgets (cmd, MAXLINE, fp) != NULL){ 
			tokens = tokenize(cmd);
			
			int temp;
			if(tokens[0][0] == '*')
				CTasks[noOfCronTasks].min = -1;
			else
				CTasks[noOfCronTasks].min = atoi(tokens[0]);
				
				
			if(tokens[1][0] == '*')
				CTasks[noOfCronTasks].hour = -1;
			else
				CTasks[noOfCronTasks].hour = atoi(tokens[1]);
					
			if(tokens[2][0] == '*')
				CTasks[noOfCronTasks].dayMon = -1;
			else
				CTasks[noOfCronTasks].dayMon = atoi(tokens[2]);
				
		    if(tokens[3][0] == '*')
				CTasks[noOfCronTasks].month = -1;
			else
				CTasks[noOfCronTasks].month = atoi(tokens[3]);	
				
			if(tokens[4][0] == '*')
				CTasks[noOfCronTasks].dayOfWeek = -1;
			else
				CTasks[noOfCronTasks].dayOfWeek = atoi(tokens[4]);			
			
			CTasks[noOfCronTasks].argv = &(tokens[5]);			
		//	printf("%s %s\n" , CTasks[noOfCronTasks].argv[0] , CTasks[noOfCronTasks].argv[1]);
			noOfCronTasks ++;
			/*
			if(CTasks[noOfCronTasks].argv == NULL)
			     CTasks[noOfCronTasks].argv = malloc(sizeof(char*)*10);
			  
			     for (int i=0;i<10;i++){
					 CTasks[noOfCronTasks].argv[i] = malloc(sizeof(char) * 1000);
					 
				 }
		//	strcpy(CTasks[noOfCronTasks].argv[ , tokens[5]);    
			*/     
			
		}
		
		
	//	printf("4: %s \n", file);
	
		doCronTasks();
	}
    else{
       int *stat = malloc(sizeof(int));
       while(1){
           wait(stat);
           if(WIFEXITED(*stat)) break;
       } 
       free(stat);
       child_process_ID =-1;
        //printf("Cron Task is set , %d \n",child_process_ID);
    }
}
	
	
	
	
	
	
	


	
int parallel(char* inputs){
	
	parallelRunning= 1;
   
	char *input = (char *)malloc(MAXLINE * sizeof(char));
	strcpy(input , inputs);
	
//	printf("Inside parallel : %s \n" , input); 

//	input[strlen(input) - 1] = '\0';
	
	pid_t parentID = getpid();
	char *cmd;
	char *cmd1 = (char *)(malloc(MAXLINE * sizeof(char)));
	char **ctokens;
	cmd = strtok(input , ":");

	
	do {
	//	printf("cmd is #%s# \n", cmd);
		strcpy(cmd1 , cmd);
		int templen = strlen(cmd1);
		cmd1[templen] = '\n';
		cmd1[templen + 1] = '\0'; 
		ctokens = tokenize(cmd1);
        execute2(ctokens); 
        cmd = strtok(NULL , ":");
	 }
	 
    while((biggestParent==1) && (cmd != NULL) && (strcmp(cmd , "") != 0) && (strcmp(cmd , "\n") != 0));
    
    free(ctokens);
    free(cmd1);	
}





int execute2(char** tokens){
   
   if(tokens==NULL){
        printf("Recieved a Null argument\n");
        return;
   }
   
    if(tokens[0] ==NULL){
        printf("Command Received a Null\n");
    }
    
    if(strcmp(tokens[0],CD) == 0){
        cd(tokens);
    }
    
    else if(strcmp(tokens[0],RUN) == 0){
       child_process_ID = fork();

       if (child_process_ID== -1) {
           perror("fork failed");
       }
       else if(child_process_ID==0){
       //    printf("Inside Child\n");
          biggestParent =0;
           parent_ID = getpid(); 
           run(tokens);
       }
       else{
        //   printf("forked\n");
       }
    }

       
    else{

       child_process_ID = fork();
       //child_process_ID == 0;
       if (child_process_ID == -1) {
           perror("fork failed");
       }
       else if(child_process_ID==0){
		   
          biggestParent =0;
         //  printf("Inside Child\n");
           otherCommands(tokens);
       }
       else{
		   //parallelPids[noOfParallel++] = child_process_ID;
		   setpgid(child_process_ID,parallelGID);
         //  printf("forked\n");
       }
//       int *stat = malloc(sizeof(int));
//       wait(stat);
//       free(stat);
    }
    child_process_ID = -1;
}



int execute3(char** tokens){ // this function is written for piped execution
    //printf("Here in 888 othercommands\n");
   if(tokens==NULL){
        printf("Recieved a Null argument\n");
        return;
   }
   
    if(tokens[0] ==NULL){
        printf("Command Received a Null\n");
    }
    
    if(strcmp(tokens[0],CD) == 0){
        cd(tokens);
    }
    
    else if(strcmp(tokens[0],RUN) == 0){

          biggestParent =0;
          parent_ID = getpid(); 
          run(tokens);
    }
    else{
        biggestParent =0;
        parent_ID = getpid();
         
        otherCommands(tokens);
    
    }
    
    exit(1);
    //printf("Here fksdljin othercommands\n");
}







int execute(char** tokens){
   if(tokens==NULL){
        printf("Recieved a Null argument\n");
        return;
   }
   
    if(tokens[0] ==NULL){
        printf("Command Received a Null\n");
    }
    
    if(strcmp(tokens[0],CD) == 0){
        cd(tokens);
    }
    
    else if(strcmp(tokens[0],RUN) == 0){
       child_process_ID = fork();

       if (child_process_ID== -1) {
           perror("fork failed");
       }
       else if(child_process_ID==0){
       //    printf("Inside Child\n");
          biggestParent =0;
           parent_ID = getpid(); 
           run(tokens);
       }
       else{
        //   printf("forked\n");
       }
       int *stat = malloc(sizeof(int));
       while(1){
           wait(stat);
           if(WIFEXITED(*stat)) break;
       } 
       free(stat);
    }

       
    else{

       child_process_ID = fork();
       //child_process_ID == 0;
       if (child_process_ID == -1) {
           perror("fork failed");
       }
       else if(child_process_ID==0){
          biggestParent =0;
         //  printf("Inside Child\n");
           otherCommands(tokens);
       }
       else{
         //  printf("forked\n");
       }
       int *stat = malloc(sizeof(int));
       while(1){
           wait(stat);
           if(WIFEXITED(*stat)) break;
       } 
       free(stat);
    }
    child_process_ID = -1;
}

















int otherCommands(char** tokens){
    
    int status = execvp(tokens[0],tokens);
    if(status==-1){

       pid_t ppid = getppid();
       printf("command not found\n");
       kill(ppid, SIGUSR1);
    
    }
    
    return 1; 

}
int cd(char** tokens){
    int a;
    a = chdir(tokens[1]); 
    if(a ==-1){
        printf("Could Not change directory, No such directory\n");
    }
    return 1;
}
