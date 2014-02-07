#ifndef EVENT_H
#define EVENT_H

#include "header.h"
#include "process.h"
#include <iostream>
class Process;
class Event {
    public:
    
    eventTypes eventType; 
    Process *p;
    int time;
    Event();
    void print();
};

struct EventPtr{
    Event *eventPtr;
    EventPtr(Event *e){
        eventPtr = e;
    }
    bool operator< (const EventPtr &e2) const
    {
    	return (eventPtr->time) > (e2.eventPtr)->time;
     }

};

struct ProcessPtr{
    Process *processPtr;
    ProcessPtr(Process *e){
        processPtr = e;
    }
    bool operator< (const ProcessPtr &e2) const
    {
    	return (processPtr->priority) < (e2.processPtr)->priority;
     }
};


#endif
