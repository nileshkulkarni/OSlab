#include<stdio.h>

int main(){
    
    int *a = malloc(sizeof(int)*100);
    *a = 10;
    int *b = malloc(sizeof(int)*100); 
    int c = (int)a;
   // printf("Typecasted check : %p %d %p \n" , a , c , (void *)c); 
    
	syscall(351,1,sizeof(int)*100,(int)b,1,1);
    printf("About to make syscall's  %d\n", (int) a);

/*
    while(1){
        printf("kakuji roxx!! \n");
        syscall(351,0,sizeof(int)*100,(int)a,1,1);
//        printf("Done with syscall  write2\n");
    }
*/
    
    printf("Done with syscall 1\n");
	printf(((*b == 10)? "Write works, b is %d \n" : "Write doesn't work , b is %d \n"),*b);
	return 1;
}