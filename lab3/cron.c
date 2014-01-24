#include "header.h"

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
extern struct cronTask *CTasks;
extern int noOfCronTasks;
int flag = 0;
int prevTime;
void SIGALARM_Handler(int sig){

    flag=1;
    alarm(1);
}

int cron(){
    
    while(1){
        if(flag==1){
            printf("Start Checking\n");
            int i;
            for(i =0;i<noOfCronTasks;i++){
                analyse(CTasks[i]);
            }
            flag=0;
        } 
    }
    return 1;

}

void runComm(char** argv){
    execute(argv);
    return;
}
int analyse(struct cronTask task){
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
   
    if((task.min == tm.tm_min || task.min == -1)){
       if(task.hour == tm.tm_hour|| task.hour ==-1){
            if(task.dayMon == tm.tm_mday||task.dayMon ==-1 ){
                if(task.month == tm.tm_mon||task.dayMon ==-1 ){
                    if(task.dayOfWeek == tm.tm_wday||task.dayOfWeek ==-1){
                            runComm(task.argv);
                        }
                    }
               }
       }
    }
                        
}

int doCornTasks(){

    if(signal(SIGALRM, SIGALARM_Handler) == SIG_ERR){
         printf("SIGINT install error\n");
         exit(1);
    }
    alarm(1);
    cron();
    return 0;
}
