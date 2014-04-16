#include <stdio.h>

int main(){
    
    
    
    FILE *fp = fopen("1.c","r");
    char *buf = malloc(10000);
    
    fseek(fp, 100, SEEK_SET);
    fread(buf, 1, 1000 , fp);

    buf[9] = '\0';
    printf("Buf is %s \n", buf);
    fclose(fp);

    int *a = malloc(sizeof(int)*1000);
    *a = 10;
    int *b = malloc(sizeof(int)*1000); 
   // printf("Typecasted check : %p %d %p \n" , a , c , (void *)c); 
    
    //printf("About to make syscall's  %d\n", (int) a);
    
    syscall(350, 40);  
    syscall(351,0,sizeof(int)*100,(int)a,15,1);
    //printf("Done with syscall 1\n");
	syscall(351,1,sizeof(int)*100,(int)b,15,1);
    if(*b==10){
        printf("Done with syscall  success!\n");
    }
    int h;
    for(h=0;h<1111;h++)
		*b = h;
	printf(((*b == 10)? "Write works, b is %d \n" : "Write doesn't work , b is %d \n"),*b);
	return 1;
}
