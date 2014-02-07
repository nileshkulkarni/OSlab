#include "scheduler.h"



Scheduler::Scheduler(){

}

void Scheduler::initialize(int t){
    timeSlice =t;

}

void Scheduler::updateIOEvent(Process * p){

            Event *e = new Event();
            e->eventType = IO_COMPLETE;
            
            e->time = clockS.time() + (p->P).phases[p->current_process_phase].io_time;

            e->p =p;
            eventManager.addEvent(e);
            cpuStopEvent->eventType = DEFAULT;
            currProcess=NULL;


}

void Scheduler::removeCurrentProcess(){
    if(!currProcess->time_left_on_cpu){
        cpu_queue.pop();
        updateIOEvent(currProcess);
    }
    else{
        Process *p =cpu_queue.front().processPtr;
        cpu_queue.push(ProcessPtr(p)); 
    }
    updateProcessQueue(); 

}

void Scheduler::updateProcessQueue(){
    Process *p1 = cpu_queue.front().processPtr; 
    if(p1==NULL){
        if(currProcess==NULL){
            printf("CPU:: I Have no Current Process running\n");
        }
        else{
            printf("CPU:: I have a current Process running, with PID %d\n",currProcess->getPid());

        }
    }
    else if(currProcess == NULL){
        currProcess= p1;
        if(clockS.time()+timeSlice < clockS.time()+p1->time_left_on_cpu){
           cpuStopEvent->time = clockS.time()+timeSlice; 
           currProcess->time_left_on_cpu = currProcess->time_left_on_cpu - timeSlice; 
        }
        else{
           cpuStopEvent->time = clockS.time()+p1->time_left_on_cpu; 
           currProcess->time_left_on_cpu = 0; 
        }
        cpuStopEvent->p=p1;
        cpuStopEvent->eventType = CPU_STOP;
    
    }
    else{
        return;
    }

}


void Scheduler::add_process(Process* p1){
    ProcessPtr *p = new ProcessPtr(p1);
    cpu_queue.push(*p);
    updateProcessQueue();
}

