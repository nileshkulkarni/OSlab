#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"


pthread_mutex_t mutex_var[NUM_THREADS];
pthread_cond_t sem_var[NUM_THREADS];
message messages[NUM_THREADS];


void *process_code(void *arg){ //reads a file for commands and adds it to messages(global variable)

	int thread_id = (int) arg;
	char file[100];
	
	
	pthread_mutex_lock(&mutex_var[thread_id]);
	
	snprintf(file, sizeof(file), "%d", thread_id);
	FILE *fp = fopen(file , "r");
	
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


void *ipc_controller(void *arg); 


int main(){
	
	
	
	
	
	
	return 0;
	
}
