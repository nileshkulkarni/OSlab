#include "header.h"
#include "eventManager.h"

EventManager::EventManager():event_queue(new Comp(true)){
    while(1){
        Event *e = event_queue.top();
        if(e->TYPE == ADMISSION_EVENT){



        }
        else if(e->TYPE == ADMISSION_EVENT){



        }
        else if(e->TYPE == ADMISSION_EVENT){



        }
        else if(e->TYPE == ADMISSION_EVENT){



        }


    }
    
}
void EventManager::addEvent(Event *e){
    event_queue.push(e);
    
    return;
}

