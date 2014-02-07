#include "scheduler.h"

Scheduler::Scheduler(){
    

}

void Scheduler::add_process(Process* p1){
    cpu_queue.push(p1);

}
