#include "header.h"

extern pid_t child_process_ID;
extern int biggestParent;

	
int parallel(char* inputs){
	
	
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
//      int *stat = malloc(sizeof(int));
//       wait(stat);
//       free(stat);
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
//       int *stat = malloc(sizeof(int));
//       wait(stat);
//       free(stat);
    }
    child_process_ID = -1;
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
       wait(stat);
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
       wait(stat);
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
