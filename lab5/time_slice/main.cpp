#include "header.h"
#include "scheduler.h"
#include "event.h"
#include <iostream>

Scheduler sch;
EventManager eventManager;
vector<process> process_list;
Event *cpuStopEvent;
Clock clockS;
struct scheduler my_scheduler;
int main(){
    process_proc_file();
    cout<<"Size of process list  "<<process_list.size()<<"\n";
    //cout<<(process_list[1].phases[1]).io_time<<endl;
    process_scheduler_file();
    //cout<<my_scheduler.levels[1].time_slice<<endl;
    for(int i =0;i<process_list.size();i++){
        Event *e = new Event();
        
        e->eventType = ADMISSION_EVENT;
      //  printf("Admission event type is % d type is %d\n ",ADMISSION_EVENT,e->eventType);
        e->time =process_list[i].admission; 
        Process *p1 = new Process(process_list[i]);
       // std::cout<<" Process with pid" << p1->getPid()<<"\n";
        e->p = p1;
        eventManager.addEvent(e);
    }
    sch.initialize(my_scheduler.levels[0].time_slice);
    cpuStopEvent = new Event();
    cpuStopEvent->time = -1;
    cpuStopEvent->p = NULL;
    cpuStopEvent->eventType= DEFAULT;
    
    eventManager.start();
}
