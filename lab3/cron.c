#include "header.h"

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>


#define task CTasks[i]

extern struct cronTask *CTasks;
extern int noOfCronTasks;
int flag = 0;
int prevTime;
void SIGALARM_Handler(int sig){
	printf("Pet Dog \n");
    flag=1;
    alarm(1);
}

int cron(){
    
    //while(1){
		
	//printf("heee %d \n" , flag);
        if(flag==1){
            printf("Start Checking %d\n" , noOfCronTasks);
            /*int i;
            for(i =0;i<noOfCronTasks;i++){
                analyse(i);
            }*/
            flag=0;
        } 
    //}
    return 1;

}

void runComm(char** argv){
    execute(argv);
    return;
}
int analyse(int i){
	
	printf("here111 : \n");
  	//printf("%s %s\n" , CTasks[i].argv[0] , CTasks[i].argv[1]);
				
	
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
    
  // runComm(task.argv); 
                        
}

int doCronTasks(){

    if(signal(SIGALRM, SIGALARM_Handler) == SIG_ERR){
         printf("SIGINT install error\n");
         exit(1);
    }
   // printf("kaam chalu \n");
    alarm(1);
    cron();
    return 0;
}
