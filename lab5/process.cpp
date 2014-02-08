#include "process.h"

Process::Process(struct process p){
	
    P.p_id = p.p_id;
	P.start_priority = p.start_priority;
	P.admission = p.admission;
	
    P.phases=p.phases;
    for(int i=0;i<P.phases.size();i++){
       printf("IO_time %d , pid is %d\n" ,P.phases[i].io_time,P.p_id);

    }
    current_process_phase = 0;
    current_iteration_no =-1;
    time_left_on_cpu = 0;
    prempted_cpu = 0;
    priority = P.start_priority;
    return;
}
void Process::print(){

    printf("Process PID %d, time left on cpu is %d , priority %d, current IO time %d \n",P.p_id,priority,time_left_on_cpu, P.phases[current_process_phase].io_time);
}
int Process::getPid(){
    return P.p_id; 

}
int Process::updateToNextCpu(int cpuDone){
    //printf("Came here updateToNextCpu \n") ;
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
                    this->print();
                    return 1;
                }
                else{
                    printf("current_process_phase %d\n ", current_process_phase);
                    this->print();
                    return 0;                     

                }
            }
            else{
                //continue with the same process phase
                //do an IO and schedule a CPU_burst 
                time_left_on_cpu = P.phases[current_process_phase].cpu_time;
                printf("Updating to the current iteration %d\n",current_iteration_no);
                this->print();
                return 1;
            }
}


int  Process::getIOTime(){
            int IO_time = P.phases[current_process_phase].io_time; 
            return IO_time;
}

