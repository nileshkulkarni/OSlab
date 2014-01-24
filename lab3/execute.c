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
//        run(tokens);
    }
    else{
 //       otherCommands(tokens);
    }
}


int cd(char** tokens){
    
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
    }
    else if(pid==0){
        int a;
        printf("Inside Child\n");
        a = chdir(tokens[1]); 
        if(a ==-1){
            printf("Could Not change directory\n");
        }
    }
    else{
        printf("inside parent %d \n",child_process_ID);
    }
}
