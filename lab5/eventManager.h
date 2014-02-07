#ifndef EVENT_MANANER_H
#define EVENT_MANAGER_H
#include <queue>
#include "header.h"
#include "clock.h"
#include "event.h"
#include "process.h"
using namespace std;

class Scheduler;
bool operator >(const EventPtr &e1,const EventPtr &e2);

class EventManager{
    public:        
    priority_queue<EventPtr> event_queue;

    EventManager();
    void addEvent(Event* );
    void start();
};

extern Scheduler scheduler;
extern EventManager eventManager;
extern Clock clockS;
extern Event* cpuStopEvent;

#endif
