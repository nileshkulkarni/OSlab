#include<stdio.h>

int main(){
    
    int *a = malloc(sizeof(int));
    *a = 10;
    int *b = malloc(sizeof(int)); 
    int c = (int)a;
   // printf("Typecasted check : %p %d %p \n" , a , c , (void *)c); 
    
    //printf("About to make syscall's  %d\n", (int) a);
    syscall(351,0,sizeof(int),(int)a,1,1);
    //printf("Done with syscall 1\n");
	syscall(351,1,sizeof(int),(int)b,1,1);
//   printf("Done with syscall 2\n");
	printf(((*b == 10)? "Write works, b is %d \n" : "Write doesn't work , b is %d \n"),*b);
	return 1;
}
