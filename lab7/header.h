#define NUM_THREADS  4
#define MAX_BUFFER_SIZE 20


enum message_type{SEND , RECEIVE};


typedef
struct message{
	char msg[100];
	message_type type;
	int receiver; //if type = SEND
}
message;

