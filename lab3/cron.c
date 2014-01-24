//#include "header.h"

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
extern struct cornTask *CTasks;
extern int noOfCornTasks;
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
            for(int i =0;i<noOfCornTasks;i++){
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
  struct tm tm = *localtime(&t)
   
    if((task.min == tm.tm_min || task.min == -1)){
       if(task.hour == tm.tm_hour|| task.hour ==-1){
            if(task.dayMon == tm.tm_mday||task.dayMon ==-1 ){
                if(task.month == tm.tm_mon||task.dayMon ==-1 ){
                    if(task.year == tm.tm_year||task.year ==-1 ){
                        if(task.dayOfWeek == tm.tm_wday||task.dayOfWeek ==-1){
                            runComm(argv);
                        }
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
    alarm(60);
    cron();
    return 0;
}
