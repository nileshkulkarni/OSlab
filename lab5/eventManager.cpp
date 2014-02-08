#include "header.h"
#include "scheduler.h"
#include "process.h"
EventManager::EventManager(){
   // event_queue(new EventComp());
    
}
void EventManager::start(){

    while(1){
        Event *e; 
        printf("Clock is  %d \n\n ",clockS.time());
        if(event_queue.size()){

            e= event_queue.top().eventPtr;
            printf("Event e time is  %d, event type is %d\n",e->time,e->eventType);
        }
        else{
            e=NULL;

        }
        printf("Event Queue size  %d \n ",event_queue.size());
        printf("CPU Stop event time is %d, and type is %d" , cpuStopEvent->time, cpuStopEvent->eventType);
        if(event_queue.size()==0 && cpuStopEvent->eventType ==DEFAULT){

            printf("Done !\n");
            break;
        }
        else if(cpuStopEvent->eventType ==DEFAULT){
            printf("No processs  to run on CPU?\n");    

        }
        else if(event_queue.size() ==0){
        //else if(){
            
            clockS.time(cpuStopEvent->time);
            printf("About to remove a process with pid e ==NULL %d\n", cpuStopEvent->p->getPid()); 
            sch.removeCurrentProcess();
            continue;
        }

        else if(cpuStopEvent->time < e->time){
            //stop the current process executing on the cpu
            clockS.time(cpuStopEvent->time);
            printf("About to remove a process with pid %d\n", cpuStopEvent->p->getPid()); 
            sch.removeCurrentProcess();

            // scheduler will push for an IO_COMPLETE Event for the process  
            continue;
        }
        if(e->eventType == ADMISSION_EVENT){
            clockS.time(e->time);
            event_queue.pop();
            Event *newEvent = new Event();
            newEvent->eventType = CPU_EXEC;
            newEvent->time = clockS.time();  
            newEvent->p = e->p;
            if((newEvent->p)->updateToNextCpu(0)){ 
            printf("Adding event \n");
            eventManager.addEvent(newEvent);
            newEvent->print();
            }
            // added a new process to the event handler for execution
        }
        else if(e->eventType == CPU_EXEC){
                clockS.time(e->time);
                event_queue.pop();
               //kk cpuStopEvent->time = clockS.time() + (e->p)->time_left_on_cpu;
               // cpuStopEvent->p = e->p;
                 
                sch.add_process(e->p);

                /// push the event to the scheduler for further processing.
        }
        else if(e->eventType == IO_COMPLETE){
                clockS.time(e->time);
                event_queue.pop();
                Event *newEvent = new Event();
                newEvent->eventType = CPU_EXEC;
                newEvent->time = clockS.time(); 
                newEvent->p = e->p;
                printf("IO _COMPELTE \n");
                if((newEvent->p)->updateToNextCpu(0)){ 
                    //printf("update for next cpu satisfied\n");
                    eventManager.addEvent(newEvent);
                    printf("event_queue size  is  %d ", event_queue.size());
                }
        }
        else if(e->eventType == COMPLETE){
                clockS.time(e->time);
                event_queue.pop();
        }
    }
}
void EventManager::addEvent(Event *e){
    struct EventPtr eve(e);
    event_queue.push(eve);
    return;
}

