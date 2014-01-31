#include "header.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#define ALARM_TIME 60

//#define task CTasks[i]

extern struct cronTask *CTasks;
extern int noOfCronTasks;
volatile sig_atomic_t flag = 0;
int prevTime;
/* interrupt handler for alarm */
void SIGALARM_Handler(int sig){
   signal(SIGALRM, SIG_IGN);
    flag=1;
    alarm(ALARM_TIME);
   signal(SIGALRM, SIGALARM_Handler);
    
}

/* routine which keeps on checking if it is time for an event to be executed*/
int cron(){
    while(1){
        if(flag==1){
            int i;
            for(i =0;i<noOfCronTasks;i++){
                analyse(CTasks[i]);
            }
        
            flag=0;
        } 
    } 
    return 1;

}
/* executes  timed commands by spawning processes in jash */
void runComm(char** argv){
    execute(argv);
    return;
}
/* checks for time and event regex*/
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
/* called from main to do cron tasks */
int doCronTasks(){

    if(signal(SIGALRM, SIGALARM_Handler) == SIG_ERR){
         printf("SIGINT install error\n");
         exit(1);
    }
    alarm(ALARM_TIME);
    flag=0;
    cron();
    return 0;
}
