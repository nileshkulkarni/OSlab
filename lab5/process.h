#ifndef PROCESS_H
#define PROCESS_H
class Process{

    public:
        struct process P;
        int current_process_phase;
        int current_iteration_no;
        int time_for_next_cpu;
        int prempted_cpu;
        int priority;
        void updateToNextCpu();

};

#endif
