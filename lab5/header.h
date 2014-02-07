#ifndef HEADER_H
#define HEADER_H


enum eventTypes{
     ADMISSION_EVENT,
     CPU_EXEC,
     IO,
     COMPLETE
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
bool operator compEvent (Event *e1,Event *e2);
bool operator compScheduler (Process *p1,Process p2);


#endif
