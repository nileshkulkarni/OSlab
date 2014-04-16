#include<stdio.h>

int main(){
    
    int *a = malloc(sizeof(int)*100);
    *a = 10;
    int *b = malloc(sizeof(int)*100); 
   // printf("Typecasted check : %p %d %p \n" , a , c , (void *)c); 
    
    //printf("About to make syscall's  %d\n", (int) a);
      
    syscall(351,0,sizeof(int)*100,(int)a,15,1);
    //printf("Done with syscall 1\n");
	syscall(351,1,sizeof(int)*100,(int)b,15,1);
    if(*b==10){
        printf("Done with syscall  success!\n");
    }
//	printf(((*b == 10)? "Write works, b is %d \n" : "Write doesn't work , b is %d \n"),*b);
	return 1;
}
