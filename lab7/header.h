#define NUM_THREADS  4
#define MAX_BUFFER_SIZE 20


enum message_type{SEND , RECIEVE};


typedef
struct message{
	char msg[100];
	message_type type;
	int receiver; //if type = RECIEVE
}
message;

