#include<stdio.h>

int main(){
    int a =0;
    int *c = malloc(sizeof(int)); 
	while(a++<10){
    a++;
    printf("Yes!!\n");
    }
    syscall(351,0,sizeof(int)*100,(int)c,5,5);
    return 1;
}
