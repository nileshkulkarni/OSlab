#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"


pthread_mutex_t mutex_var[NUM_THREADS];
pthread_cond_t sem_var[NUM_THREADS];
message messages[NUM_THREADS];


void *process_code(void *arg){ //reads a file for commands and adds it to messages(global variable)

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


void *ipc_controller(void *arg); 


int main(){
	
	
	
	
	
	
	return 0;
	
}
