#include "process.h"

Process::Process(struct process p){
	
    P.p_id = p.p_id;
	P.start_priority = p.start_priority;
	P.admission = p.admission;
	P.phases=p.phases;
    
    current_process_phase = 0;
    current_iteration_no =-1;
    time_left_on_cpu = 0;
    prempted_cpu = 0;
    priority = P.start_priority;
    return;
}

int Process::getPid(){
    return P.p_id; 

}
int Process::updateToNextCpu(int cpuDone){
    printf("Came here updateToNextCpu \n") ;
    if(cpuDone == 0){
        //schedule to next iteration
            int IO_time = P.phases[current_process_phase].io_time; 
            current_iteration_no++ ;
            if(current_iteration_no == P.phases[current_process_phase].iterations){
                //this process phase completed, go to the next process phase.
                printf("Updating to the next phase %d\n",this->getPid());
                current_process_phase++;
                current_iteration_no =0;
                if(current_process_phase < (P.phases).size()){
                    // move to next phase 
                    time_left_on_cpu =  P.phases[current_process_phase].cpu_time; 
                    return 1;
                }
                else{
                    return 0;                     

                }
            }
            else{
                //continue with the same process phase
                //do an IO and schedule a CPU_burst 
                time_left_on_cpu = P.phases[current_process_phase].cpu_time;
                return 1;
            }
    }
}


int  Process::getIOTime(){
            int IO_time = P.phases[current_process_phase].io_time; 
            return IO_time;
}

