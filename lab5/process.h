#ifndef PROCESS_H
#define PROCESS_H
#include "header.h"
#include <iostream>
#include <stdio.h>
//#include "event.h"

class Process{
    public:
        
        process P;
        int current_process_phase;
        int current_iteration_no;
        int time_left_on_cpu;
        int time_start_on_cpu;
        int prempted_cpu;
        int priority;
        
        Process(struct process);
        
        void updateToNextCpu(int);
        int getIOTime();
        int getPid();
};

#endif
