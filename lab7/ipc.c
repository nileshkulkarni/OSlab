#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "header.h"

message msg[NUM_THREADS];
pthread_mutex_t mutex_var[NUM_THREADS];
pthread_cond_t sem_var[NUM_THREADS];
message messages[MAX_BUFFER_SIZE];
int empty_block;
int start_block;
int bufferFull;

void* process_code(void *arg){ //reads a file for commands and adds it to messages(global variable)

	int thread_id = (int) arg;
	char file[100];
	
	
	pthread_mutex_lock(&mutex_var[thread_id]);
	
	snprintf(file, sizeof(file), "%d", thread_id);
	FILE *fp = fopen(file , "r");
	
	printf("Hello from thread %d \n", thread_id);
	
	char op[256];
	char receiver[10];
	
	while (fgets(op, sizeof(op), fp)){
		strip(op);
		if(strcmp(op , "S") == 0){ 
			fgets(messages[thread_id].msg, sizeof(messages[thread_id].msg), fp);
			strip(messages[thread_id].msg);
			fgets(receiver, sizeof(receiver), fp);
			strip(receiver);
			messages[thread_id].receiver = atoi(receiver);
			messages[thread_id].type = SEND;
		}	
		else if(strcmp(op , "R") == 0){ 
			messages[thread_id].type = RECEIVE;
		}
		else{
			printf("Invalid file : Aborting Thread %d \n" , thread_id);;
			exit(0);
		}
	
	    pthread_cond_wait(&sem_var[thread_id], &mutex_var[thread_id]); //mutex is temporarily locked at this point
		
		if(strcmp(op , "R") == 0){
			printf("Received message(--%s--)  at Thread: %d from Thread: %d \n" , 
								messages[thread_id].msg , thread_id , messages[thread_id].receiver);
		}
	
	}
	
	pthread_mutex_unlock(&mutex_var[thread_id]);
	pthread_exit((void*) 0);
}

void* ipc_controller(void *arg){
	
    printf("Hello from ipc %d \n", (int)arg);
    
    
    while(1){
        int i =0;
        
        for(i=0;i<4;i++){
           
            if(pthread_mutex_trylock(&mutex_var[i]) == 0){
                // updated struct is here now, read it.
                if(msg[i].type == SEND){ 
                    if(bufferFull ==0){
                      messages[empty_block] = msg[i]; 
                      empty_block = (empty_block+1)%MAX_BUFFER_SIZE;
                      if(empty_block == start_block){
                            bufferFull = 1;
                      }
                    printf("Process %d wants to send a message\n ",i); 
                    pthread_cond_signal(&sem_var[i]); //mutex is temporarily locked at this point
                    }
                    else{
						printf("Process %d wants to send a message but the buffer is full! Sorry\n ",i); 
                       continue; //dont signal the process here let it wait for the buffer to get empty
                    }
                }
                if(msg[i].type ==RECEIVE){
                   int j = start_block;
                   for(;j!=empty_block;j=(j+1)%MAX_BUFFER_SIZE){
                        if(messages[j].receiver == i){
                            msg[i] = messages[j];
                            if(j ==start_block){
                                start_block = (start_block +1) %MAX_BUFFER_SIZE;
                                if(start_block !=empty_block){
                                    bufferFull = 0;
                                }
                                printf("Process %d wants to recieve a message\n ",i); 
                            
                            }
                            else{
                                int k=0;
                                for(k=j;k!=(empty_block-1 + MAX_BUFFER_SIZE)%MAX_BUFFER_SIZE;k=((k+1)%MAX_BUFFER_SIZE)){
                                    messages[k] = messages[k+1];
                                }
                                empty_block = k;
                                if(empty_block!=start_block){
                                    bufferFull =0;
                                }
                            }
                            pthread_cond_signal(&sem_var[i]); //mutex is temporarily locked at this point
                            break;
                        }
                   }
                }
            }
        }
    }
} 

int main(){
	
	pthread_t threads[NUM_THREADS+1];
	pthread_attr_t attr;
	int i;
	  /* Initialize mutex and condition variable objects */
	for(i=0;i<NUM_THREADS;i++){ 
			  pthread_mutex_init(&mutex_var[i], NULL);
			  pthread_cond_init (&sem_var[i], NULL);
	}
	  /* For portability, explicitly create threads in a joinable state */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	for(i=0;i<NUM_THREADS;i++){  
	  pthread_create(&threads[i], &attr, process_code, (void *)i);
	}	
	i=4;
	
	pthread_create(&threads[i], &attr, ipc_controller , (void *)i);
	
	 /* Wait for all threads to complete */
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	
	
	pthread_attr_destroy(&attr);
	for(i=0; i<NUM_THREADS;i++){
		pthread_mutex_destroy(&mutex_var[i]);
		pthread_cond_destroy(&sem_var[i]);
	}
	pthread_exit (NULL);	
	
	
	return 0;
	
}
