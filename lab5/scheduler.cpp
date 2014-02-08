#include "scheduler.h"



Scheduler::Scheduler(){
        

}

void Scheduler::updateIOEvent(Process * p){

            Event *e = new Event();
            e->eventType = IO_COMPLETE;
             
            e->time = clockS.time() + (p->P).phases[p->current_process_phase].io_time;
            printf("Current Phase is %d  io_time is %d\n", p->current_process_phase,(p->P).phases[p->current_process_phase].io_time); 
            e->p =p;
            eventManager.addEvent(e);
            printf("Inside IO update \n");
            e->print();
            currProcess = NULL;


}

void Scheduler::removeCurrentProcess(){
    Process* p1 = cpu_queue.top().processPtr;
    printf("Removing current process \n");
    p1->print();
    printf("\n");
    if(currProcess->getPid() == p1->getPid()){
       // printf("No process updates Require, No process at higher priority \n");
        if(clockS.time() == cpuStopEvent->time){
            // stop executing the currentProcess
            // create a new event to mark completion on IO for the process;
            updateIOEvent(p1); // this will add a new IO event in the scheduler
            cpu_queue.pop();
            if(cpu_queue.size()){
                p1 = cpu_queue.top().processPtr; // this the new process which is going to execute on the cpu    
                printf("Removing current process2 \n");
                p1->print();
                printf("\n");
            }
            else
                p1 =NULL;
            
            if(p1 != NULL){
                cpuStopEvent->time =  clockS.time() + p1->time_left_on_cpu;
                 
                cpuStopEvent->p = p1;
                cpuStopEvent->eventType =CPU_STOP;   
                currProcess = p1;
                currProcess->time_start_on_cpu = clockS.time();
            }
            else{
                cpuStopEvent->eventType = DEFAULT;
            }
        }
    }
    else{
        printf("should not come here something just went terribly wrong\n");
    }


}

void Scheduler::updateProcessQueue(){
    Process *p1 = cpu_queue.top().processPtr;
    printf("inside update Process QUEUE \n"); 
    if(p1==NULL){
        if(currProcess == NULL){
           printf("CPU :: I have no current Process running\n"); 
        }
        else{
           printf("CPU :: I have  current Process running, with PID %d \n", currProcess->getPid()); 
        }
    }
    else if(currProcess == NULL){
        p1->print();
      //  printf("We have a new process to execute, Adding a new process as current process \n");
        currProcess = p1;
        currProcess->time_start_on_cpu = clockS.time(); 
        cpuStopEvent->time =  clockS.time() + p1->time_left_on_cpu;
        cpuStopEvent->p = p1;
        cpuStopEvent->eventType =CPU_STOP;    

    }
    
    else if(currProcess->getPid() == p1->getPid()){
        printf("inside update process queue, curr proces = p1\n ");
        p1->print();
        printf("No process updates Require, No process at higher priority \n");
        
    }
    else{
        printf("Process with higher priority in the queue \n");
        printf("printing p1 in premepted state \n");
        p1->print();
        p1->time_start_on_cpu = clockS.time();
        //if(p1->updateToNextCpu(0)){
            currProcess->time_left_on_cpu = clockS.time() - currProcess->time_start_on_cpu ; 
            currProcess->prempted_cpu = 1;
             // this will update the time left on cpu
            printf("****\n");
            p1->print();
            cpuStopEvent->time = clockS.time() + p1->time_left_on_cpu;
            printf("Stop Event Time is %d\n", cpuStopEvent->time);
            cpuStopEvent->p = p1;
            currProcess = p1;
            printf("Printin current process, when it was prempted\n");
            currProcess->print();
       /* }
        else{
            printf("Removing a obsolete process from the queue\n");
            cpu_queue.pop();
        }*/
    }
}


void Scheduler::add_process(Process* p1){
    ProcessPtr *p = new ProcessPtr(p1);
    printf("Inside add_process \n");
    p1->print();
    cpu_queue.push(*p);
    updateProcessQueue();
}

