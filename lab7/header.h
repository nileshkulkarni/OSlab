#define NUM_THREADS  2
#define MAX_BUFFER_SIZE 20


//enum message_type{SEND , RECEIVE};
#define SEND 1
#define RECEIVE 0

void strip(char *s){
	int i = strlen(s)-1;
	while(i>=0 && s[i] == '\n'){
		s[i] = '\0';
		i--;
	}
}




typedef
struct message{
	char msg[100];
	int type;
	int sender;
	int receiver; //if type = SEND
}
message;

