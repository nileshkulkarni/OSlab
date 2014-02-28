#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


#define NUM_THREADS  4
#define MAX_BUFFER_SIZE 20





typedef
struct message{
	char *msg[100];
	int receiver;
	int sender;
}
message;

message msg[4];
pthread_mutex_t mutex_var[NUM_THREADS];
pthread_cond_t sem_var[NUM_THREADS];

void* process_code(void *arg){ //reads a file for commands and adds it to messages(global variable)

	int thread_id = (int) arg;
	
	

}

void* ipc_controller(void *arg){
    while(1){
        int i =0;
        
        for(i=0;i<4;i++){
           
            if(pthread_mutex_trylock(mutex_var[i]) == 0){
                // updated struct is here now, read it.
                if(msg[i].type == 0){ 
                       

                }
                if(msg[i].type ==1){


                }
                pthread_cond_signal(sem_var[i]); //mutex is temporarily locked at this point
            }


        }
        







    }
    
    
}:; 


int main(){
	
	
	
	
	
	
	return 0;
	
}
