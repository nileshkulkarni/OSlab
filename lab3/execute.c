#include "header.h"

extern pid_t child_process_ID;


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
           printf("Inside Child\n");
          run(tokens);
       }
       else{
           printf("forked\n");
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
           printf("Inside Child\n");
           otherCommands(tokens);
       }
       else{
           printf("forked\n");
       }
       int *stat = malloc(sizeof(int));
       wait(stat);
       free(stat);
    }
    child_process_ID = -1;
}

int otherCommands(char** tokens){

    printf("Get pid %d \n", getpid());
    while(1);
    int status = execvp(tokens[0],tokens);
    return 1; 


}
int cd(char** tokens){
    int a;
    printf("Inside Child\n");
    a = chdir(tokens[1]); 
    if(a ==-1){
        printf("Could Not change directory\n");
    }
    return 1;
}
