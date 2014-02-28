#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


#define NUM_THREADS  3
#define MAX_BUFFER_SIZE 20


typedef
struct message{
	char *msg[100];
	int thread_id;
	message *next_message;
}
message;


message *messages;
int CurrBufferSize;


void *process_code(void *arg); //reads a file for commands and adds it to messages(global variable)



void *ipc_controller(void *arg); 


int main(){
	
	
	
	
	
	
	return 0;
	
}
