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
    cout<<(process_list[1].phases[1]).io_time<<endl;
    process_scheduler_file();
    cout<<my_scheduler.levels[1].time_slice<<endl;
    for(int i =0;i<process_list.size();i++){
        Event *e = new Event();
        e->eventType = ADMISSION_EVENT;
        e->time =process_list[i].admission; 
        Process *p1 = new Process(process_list[i]);
        e->p = p1;
        eventManager.addEvent(e);
    }
    eventManager.start();
}
