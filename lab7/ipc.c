#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"

message msg[NUM_THREADS];
pthread_mutex_t mutex_var[NUM_THREADS];
pthread_cond_t sem_var[NUM_THREADS];
message* messages[MAX_BUFFER_SIZE];
int empty_block;
int start_block;
int bufferFull;

void* process_code(void *arg){ //reads a file for commands and adds it to messages(global variable)

	int thread_id = (int) arg;
	pthread_mutex_lock(&mutex_var[thread_id]);
	
	FILE *fp = fopen(itoa(thread_index) , "r");
	
	char op[256];
	char recSend;
	
	while (fgets(op, sizeof(op), fp)){
		strip(op);
		if(strcmp(op , "R") == 0){ 
			fgets(messages[thread_id].msg, sizeof(messages[thread_id].msg), fp);
			fgets(recSend, sizeof(recSend), fp);
		}	
		
		
	}
}

void* ipc_controller(void *arg){
    while(1){
        int i =0;
        
        for(i=0;i<4;i++){
           
            if(pthread_mutex_trylock(mutex_var[i]) == 0){
                // updated struct is here now, read it.
                if(msg[i].type == SEND){ 
                    if(bufferFull ==0){
                      messages[empty_block] = msg[i]; 
                      empty_block = (empty_block+1)%MAX_BUFFER_SIZE;
                      if(empty_block == start_block){
                            bufferFull = 1;
                      }
                    pthread_cond_signal(sem_var[i]); //mutex is temporarily locked at this point
                    }
                    else{
                       continue; //dont signal the process here let it wait for the buffer to get empty
                    }
                }
                if(msg[i].type ==RECIEVE){
                   int j = start_block;
                   for(;j!=empty_block;j=(j+1)%MAX_BUFFER_SIZE){
                        if(buffer[j].receiver == i){
                            msg[i] = buffer[j];
                            int k=0;
                            if(j ==start_block){
                                start_block = (start_blocki +1) %MAX_BUFFER_SIZE
                                if(start_block !=empty_block){
                                    bufferFull = 0;
                                }
                            
                            }
                            else{

                                for(k=j;k!=(empty_block-1 + MAX_BUFFER_SIZE)%MAX_BUFFER_SIZE;k=((k+1)%MAX_BUFFER_SIZE)){
                                    buffer[k] = buffer[k+1];
                                }
                                empty_block = k;
                                if(empty_block!=start_block){
                                    bufferFull =0;
                                }
                            }
                            pthread_cond_signal(sem_var[i]); //mutex is temporarily locked at this point
                            break;
                        }
                   }
                }
            }
        }
    }
} 

int main(){
	
	
	
	
	
	
	return 0;
	
}
