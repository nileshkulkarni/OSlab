#include "header.h"
extern int ctrlCFlag;

extern int biggestParent;
void SIGINT_handler(int sig){
	ctrlCFlag = 1;
    if(signal(SIGINT, SIGINT_handler) == SIG_ERR){
        printf("SIGINT install error\n");
        exit(1);
    }
    
    if(getpid() == parent_ID){
    //    printf("Killed Process with PID %d \n",child_process_ID);
        if(child_process_ID != -1){
     //      printf("Killed Process with PID %d \n",child_process_ID);
           kill(child_process_ID,SIGKILL); 
        }
        else{
        }
    }
    printf("\n");
    fflush(stdout);
    return;
}
void SIGUSR1_handler(int sig){
	if(signal(SIGUSR1, SIGUSR1_handler) == SIG_ERR){
        printf("SIGINT install error\n");
        exit(1);
    }
    if(child_process_ID != -1){
       // printf("parent pid  %d Killed Process with PID %d \n",parent_ID,child_process_ID);
        kill(child_process_ID,SIGKILL);
        pid_t par = getppid();
        if(!biggestParent){
            kill(par,SIGUSR1);
        }
    }

   // fflush(stdout);
  //  printf("%d \n" ,getpid());
    return;
}
