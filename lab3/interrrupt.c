#include "header.h"


void SIGINT_handler(int sig){
    if(getpid() == parent_ID){
        //printf("Killed Process with PID %d \n",child_process_ID);
        if(child_process_ID != -1){
          // printf("Killed Process with PID %d \n",child_process_ID);
           kill(child_process_ID,SIGTERM); 
        }
        else{
        }
    }
    printf("\n$ ");
    fflush(stdout);
  //  printf("%d \n" ,getpid());
    return;
}
void SIGUSR1_handler(int sig){
    if(child_process_ID != -1){
        kill(child_process_ID,SIGTERM); 
    } 
    printf("command not found\n");
   // fflush(stdout);
  //  printf("%d \n" ,getpid());
    return;
}
