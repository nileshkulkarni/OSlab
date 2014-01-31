#include "header.h"


command  parse(char *comm){

	int i=0;
	
	command ret;
	ret.nTokens = 0;
	
	char **tokens = (char **) malloc(MAXLINE*sizeof(char**));
 
	int tokenNo = -1;
	int tokenIndex = -1;

	int flag = 1;
   
    while(comm[i] != '\0'){
		
		printf("** %c *** %d\n" , comm[i] , tokenNo);
		
		if(comm[i] == '>' || comm[i] == '<' || comm[i] == '|'){
			
			if(flag == 2){
				ret.nTokens ++;
				if((tokenIndex != -1) && (tokenNo != -1)){
					tokens[tokenNo][++tokenIndex]  = '\n';
					tokens[tokenNo][++tokenIndex]  = '\0';
				}
				tokenIndex = -1;
				tokens[++tokenNo] = (char *)malloc(1000*sizeof(char));
				flag = 1;
			}	
		 }
		
		
		else{ 
			if(flag == 1){
				ret.nTokens ++;
				if((tokenIndex != -1) && (tokenNo != -1)){
					tokens[tokenNo][++tokenIndex]  = '\0';
				}
				tokenIndex = -1;
				tokens[++tokenNo] = (char *)malloc(1000*sizeof(char));
				flag = 2;
			}
		}
			
			
			tokens[tokenNo][++tokenIndex] = comm[i];
			i++;
	}
	tokens[tokenNo][++tokenIndex]  = '\0';
	ret.tokens = tokens;
	return ret;	
}