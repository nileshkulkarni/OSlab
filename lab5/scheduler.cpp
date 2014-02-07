#include "scheduler.h"



Scheduler::Scheduler(){
        

}

void Scheduler::updateIOEvent(Process * p){

            Event *e = new Event();
            e->eventType = IO_COMPLETE;
            
            e->time = clockS.time() + (p->P).phases[p->current_process_phase].io_time;

            e->p =p;
            printf("Added IO event here\n");
            eventManager.addEvent(e); 


}

void Scheduler::removeCurrentProcess(){
    Process* p1 = cpu_queue.top().processPtr;
    if(currProcess->getPid() == p1->getPid()){
       // printf("No process updates Require, No process at higher priority \n");
        printf("Removing a process from cpu\n");
        if(clockS.time() == cpuStopEvent->time){
            // stop executing the currentProcess
            // create a new event to mark completion on IO for the process;
            updateIOEvent(p1); // this will add a new IO event in the scheduler
            cpu_queue.pop();
            p1 = cpu_queue.top().processPtr; // this the new process which is going to execute on the cpu    
            if(p1 != NULL){
                cpuStopEvent->time =  clockS.time() + p1->time_left_on_cpu;
                cpuStopEvent->p = p1;
                cpuStopEvent->eventType =CPU_STOP;    
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
    
    if(p1==NULL){
        printf("No new process to execute\n");
        if(currProcess == NULL){
           printf("CPU :: I have no current Process running\n"); 
        }
        else{
           printf("CPU :: I have  current Process running, with PID %d \n", currProcess->getPid()); 
        }
    }
    if(currProcess == NULL){
        printf("We have a new process to execute, Adding a new process as current process \n");
        
        currProcess = p1;

    }
    
    else if(currProcess->getPid() == p1->getPid()){
        printf("No process updates Require, No process at higher priority \n");
        if(clockS.time()==cpuStopEvent->time){
            // stop executing the currentProcess
            // create a new event to mark completion on IO for the process;
            updateIOEvent(p1); // this will add a new IO event in the scheduler
            cpu_queue.pop();
            p1 = cpu_queue.top().processPtr; // this the new process which is going to execute on the cpu    
            if(p1 != NULL){
                cpuStopEvent->time =  clockS.time() + p1->time_left_on_cpu;
                cpuStopEvent->p = p1;
                cpuStopEvent->eventType =CPU_STOP;    
            }
            else{
                cpuStopEvent->eventType = DEFAULT;
            }
        }
    }
    else{
        printf("Process with higher priority in the queue \n");
        currProcess->time_left_on_cpu = clockS.time()-currProcess->time_start_on_cpu ; 
        currProcess->prempted_cpu = 1; 
        p1->time_start_on_cpu = clockS.time();
        p1->updateToNextCpu(0); // this will update the time left on cpu
        cpuStopEvent->time = clockS.time() + p1->time_left_on_cpu;  
        cpuStopEvent->p = p1;
        currProcess = p1;
    }
}


void Scheduler::add_process(Process* p1){
    ProcessPtr *p = new ProcessPtr(p1);
    cpu_queue.push(*p);
    updateProcessQueue();
}
