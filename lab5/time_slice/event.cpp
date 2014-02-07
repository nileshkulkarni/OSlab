#include "event.h"

Event::Event(){
    eventType = DEFAULT; 
    p= NULL;
    time =0;
}
void Event::print(){
    printf("Process PID %d ,Event Type %d, Event Time %d\n",p->getPid(),eventType, time);

}
