#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "header.h"

message msg[NUM_THREADS];
pthread_mutex_t mutex_var[NUM_THREADS];
pthread_cond_t sem_var[NUM_THREADS];

message messages[MAX_BUFFER_SIZE];
int lockedMutex[NUM_THREADS];

int empty_block;
int start_block;
int bufferFull;
int deadlock_detected=0;




void destroy_mutex(){
	int i;
	for(i=0; i<NUM_THREADS;i++){
		pthread_mutex_destroy(&mutex_var[i]);
		pthread_cond_destroy(&sem_var[i]);
	}
}




void* process_code(void *arg){ //reads a file for commands and adds it to messages(global variable)

	int thread_id = (int) arg;
	char file[100];
	
	
	pthread_mutex_lock(&mutex_var[thread_id]);
	
	snprintf(file, sizeof(file), "%d", thread_id);
	FILE *fp = fopen(file , "r");
	
	printf("Hello from thread %d \n", thread_id);
	
	char op[256];
	char receiver[10];
	msg[thread_id].sender = thread_id;
	
	while (fgets(op, sizeof(op), fp)){
		
		if(deadlock_detected==1) break;
		
		strip(op);
	#if(DEBUG)
		printf("Thread %d %s \n", thread_id ,op);
	#endif
		if(strcmp(op , "S") == 0){ 
			fgets(msg[thread_id].msg, sizeof(msg[thread_id].msg), fp);
			strip(msg[thread_id].msg);
			fgets(receiver, sizeof(receiver), fp);
			strip(receiver);
			msg[thread_id].receiver = atoi(receiver);
			msg[thread_id].type = SEND;
				msg[thread_id].sender = thread_id;

		}	
		else if(strcmp(op , "R") == 0){ 
			msg[thread_id].type = RECEIVE;
		}
		else{
			printf("Invalid file : Aborting Thread %d \n" , thread_id);
			break;
		}
	//	printf("Thread %d is waiting \n", thread_id ,op);
	    pthread_cond_wait(&sem_var[thread_id], &mutex_var[thread_id]); //mutex is temporarily locked at this point
	//	printf("Thread %d has waited \n", thread_id ,op);
	
	#if(DEBUG)	
		if(strcmp(op , "R") == 0){
			printf("Received message(--%s--)  at Thread: %d from Thread: %d \n" , 
								msg[thread_id].msg , thread_id , msg[thread_id].sender);
		}
		else if(strcmp(op , "S") == 0){
			printf("I am thread %d and msg has been sent \n",thread_id);
		}
	#endif
	}

	msg[thread_id].type = -1;
	printf("Thread %d is exiting \n", thread_id);
	pthread_mutex_unlock(&mutex_var[thread_id]);
	pthread_exit((void*) 0);
}

void* ipc_controller(void *arg){
	
    printf("Hello from ipc %d \n", (int)arg);
    
    start_block =0;
    empty_block =0;
    while(1){
        int i =0;
        int deadlock = 0;
        int flag2=0;
        for(i=0;i<NUM_THREADS;i++){
            if(pthread_mutex_trylock(&mutex_var[i]) == 0){
				lockedMutex[i]=1;
            #if(DEBUG)
                printf("IPC :: Wait for thread %d\n",i);
                printf("OP is %d \n" ,msg[i].type);
                // updated struct is here now, read it.
            #endif
                if(msg[i].type == SEND){ 
                    if(bufferFull ==0){
                      messages[empty_block] = msg[i]; 
                      empty_block = (empty_block+1)%MAX_BUFFER_SIZE;
                      if(empty_block == start_block){
                            bufferFull = 1;
                      }
                    printf("Message Sent: %d, %d, %s\n ", i , msg[i].receiver , msg[i].msg);
                    pthread_cond_signal(&sem_var[i]); //mutex is temporarily locked at this point
                    lockedMutex[i]=0;
                    msg[i].type =-1;
                    pthread_mutex_unlock(&mutex_var[i]);
                    }
                    else{
	//					printf("Process %d wants to send a message but the buffer is full! Sorry\n ",i); 
                        deadlock++;
                        lockedMutex[i]=0;
                        pthread_mutex_unlock(&mutex_var[i]);
                        continue; //dont signal the process here let it wait for the buffer to get empty
                    }
                }
                else if(msg[i].type ==RECEIVE){
                   int j = start_block;
                   int flag = 0;
                   for(;j!=empty_block;j=(j+1)%MAX_BUFFER_SIZE){
                        if(messages[j].receiver == i){
                            strcpy(msg[i].msg ,  messages[j].msg);
                            msg[i].sender = messages[j].sender;
                            //msg[i].type = RECEIVE;
                           printf("Message Received: %d, %d, %s\n ",messages[j].sender , i,messages[j].msg); 
                            if(j ==start_block){
                                start_block = (start_block +1) %MAX_BUFFER_SIZE;
                                if(start_block !=empty_block){
                                    bufferFull = 0;
                                }
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
                            flag=1;
                            pthread_cond_signal(&sem_var[i]); //mutex is temporarily locked at this point
                            lockedMutex[i]=0;
                            pthread_mutex_unlock(&mutex_var[i]);
                            break;
                        }
                   }
                   
                   if(flag==0){
					deadlock++;
					lockedMutex[i]=0;
				//	pthread_cond_signal(&sem_var[i]); //mutex is temporarily locked at this point  
					pthread_mutex_unlock(&mutex_var[i]);
					}
                }
                
                else if(msg[i].type ==-1){
			#if(DEBUG)
					printf("No request from thread %d \n",i);
			#endif
				//	deadlock++;
					continue;
				}
				
			   else{
				    flag2++;
				    deadlock++;
//					printf("%d \n" , msg[i].type);
					lockedMutex[i]=0;
					pthread_mutex_unlock(&mutex_var[i]);
				}	
            }
        }
        
        //printf("deadlock is %d\n",deadlock);
        if(deadlock==NUM_THREADS){
			continue;
			printf("Exiting IPC Controller \n");
			deadlock_detected = 1;
			int h;
			int flag1;
			for(h=0;h<NUM_THREADS;h++){
				if(lockedMutex[h]==1){
  					  flag1 =1;
				      pthread_cond_signal(&sem_var[i]); //mutex is temporarily locked at this point
                      pthread_mutex_unlock(&mutex_var[i]);
				}
			}
			if(flag2 != 4)
				printf("Deadlock Detected\n");
			else
				printf("Completed Message Passing No more messages\n");
			pthread_exit (NULL);	
		}
		
    }
} 

int main(){
	
	pthread_t threads[NUM_THREADS+1];
	pthread_attr_t attr;
	int i;
	  /* Initialize mutex and condition variable objects */
	for(i=0;i<NUM_THREADS;i++){
			  lockedMutex[i]=0;
			  pthread_mutex_init(&mutex_var[i], NULL);
			  pthread_cond_init (&sem_var[i], NULL);
	}
	  /* For portability, explicitly create threads in a joinable state */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	for(i=0;i<NUM_THREADS;i++){  
	  pthread_create(&threads[i], &attr, process_code, (void *)i);
	}	
	i=NUM_THREADS;
	
	pthread_create(&threads[i], &attr, ipc_controller , (void *)i);
	
	 /* Wait for all threads to complete */
	
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
		msg[i].type = -2;
	}
	
	
	
	pthread_attr_destroy(&attr);
//	destroy_mutex();
	
	printf("Exiting main Thread \n");
	
	pthread_exit (NULL);	
	
	
	return 0;
	
}
