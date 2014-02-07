#include "clock.h"

Clock::Clock(){

    curr_time =0;

}
int Clock::time(){

    return curr_time;  

}
int Clock::time(int t){

    curr_time =t;  

}
