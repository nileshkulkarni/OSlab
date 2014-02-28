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


pthread_mutex_t mutex_var[NUM_THREADS];
pthread_cond_t sem_var[NUM_THREADS];

void *process_code(void *arg){ //reads a file for commands and adds it to messages(global variable)

	int thread_id = (int) arg;
	
	

}

void *ipc_controller(void *arg); 


int main(){
	
	
	
	
	
	
	return 0;
	
}
