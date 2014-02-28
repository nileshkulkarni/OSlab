#define NUM_THREADS  4
#define MAX_BUFFER_SIZE 20


enum message_type{SEND , RECEIVE};



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
	message_type type;
	int receiver; //if type = SEND
}
message;

