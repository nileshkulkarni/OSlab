#ifndef HEADER_H
#define HEADER_H
#include <vector>
using namespace std;

enum eventTypes{
     ADMISSION_EVENT,
     CPU_EXEC,
     IO_COMPLETE, COMPLETE, CPU_STOP,
     DEFAULT
};

struct process_phase{
	int iterations;
	int cpu_time;
	int io_time;
};

struct process{
	int p_id;
	int start_priority;
	int admission;
	vector<process_phase> phases;
};

struct sc_level{
	int priority;
	int time_slice;
};

struct scheduler{
	int no_levels;
	vector<sc_level> levels;
};



void process_proc_file();
void process_scheduler_file();


#endif
