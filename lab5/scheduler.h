#ifndef SCHEDULER_H
#define SCHEDULER_H
#include<queue>
#include "header.h"
using namespace std;
class Scheduler{
    std::priority_queue<Process*> cpu_queue; 
    
    Process* prevProcess;
    void add_process();
    
};
#endif
