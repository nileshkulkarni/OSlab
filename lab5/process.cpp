#include "process.h"
#include "header.h"
Process::Process(struct process p){
	P.p_id = p.p_id;
	P.start_priority = p.start_priority;
	P.admission = p.admission;
	P.phases=p.phases;
    
    current_process_phase = 0;
    current_iteration_no =0;
    time_for_next_cpu = 0;
    prempted_cpu = 0;
    priority = P.start_priority;
    return;
}


void Process::updateToNextCpu(int cpuDone){
    if(cpuDone == P.phases[current_process_phase].cpu_time){
        //schedule to next iteration
            int IO_time = P.phases[current_process_phase].IO_time; 
            current_iteration_no++ ;
            if(current_iteration_no == P.phases[current_process_phase].iterations){
                //this process phase completed, go to the next process phase.
                current_process_phase++;
                current_iteration_no =0;
                if(current_process_phase < phases.size()){
                    // move to next phase 
                    time_for_next_cpu = clock.getTime() + P.phases[current_process_phase].IO_time; 
                }
            }
            else{
                //continue with the same process phase
                //do an IO and schedule a CPU_burst 
                time_for_next_cpu = clock.getTime() + IO_time;
            }
    }
    else{
        //current IO has not being completed yet, 
        //continue with left time
        prempted_cpu = P.phases[current_process_phase].cpu_time - cpuDone; 
    }

    eventHandler.push_back(P);
}

