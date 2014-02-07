#include "header.h"
#include "process.h"
#include "event.h"

bool operator compEvent (Event *e1,Event *e2){
    return e1->time < e2->time;
}

bool operator compScheduler (Process *p1,Process p2){
    return p1.time_for_next_cpu < p2.time_for_next_cpu;
}
/*
class Comp
{
    bool time;
    public:
    mycomparison(const bool& time) {
        reverse=time;
    }
     bool operator() (const Process* p1, const Process* p2) const
     {
         if (time) 
             return (p1->time_for_next_cpu < p2->time_for_next_cpu );
         else 
             return (p1->priority < p2->priority);
      }
};
*/
