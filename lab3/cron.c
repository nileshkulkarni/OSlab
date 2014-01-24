//#include "header.h"

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

int flag = 0;
int prevTime
void SIGALARM_Handler(int sig){

    flag=1;
    alarm(3);
}

int cron(){
    
    while(1){
        if(flag==1){
            printf("Start Checking\n");
            flag=0;
        } 
    }
    return 1;

}


int analyse(struct cronTask task){
     



}
int main(){

    if(signal(SIGALRM, SIGALARM_Handler) == SIG_ERR){
         printf("SIGINT install error\n");
         exit(1);
    }
    alarm(3);

    cron();
    return 0;
}
