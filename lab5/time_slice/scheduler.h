#ifndef SCHEDULER_H
#define SCHEDULER_H
#include<queue>
#include<stdio.h>
#include "header.h"
#include "process.h"
#include "event.h"
#include "clock.h"
#include "eventManager.h"
using namespace std;


class Scheduler{
    public:
    int timeSlice; 
    std::queue<ProcessPtr> cpu_queue; 
    Scheduler(); 
    Process* currProcess;
    void add_process(Process *);
    void removeCurrentProcess();
    void updateProcessQueue(); 
    void updateIOEvent(Process *);
    void initialize(int t);
};

extern Scheduler sch;
extern EventManager eventManager;
extern Clock clockS;
extern Event* cpuStopEvent;
#endif
