#include <stdio.h>

int main(){
    
    
    
    FILE *fp = fopen("1.c","r");
    char *buf = malloc(1000);
    
    fseek(fp, 100, SEEK_SET);
    fread(buf, 1, 9, fp);

    buf[9] = '\0';
    printf("Buf is %s \n", buf);
    fclose(fp);
    
    
    fp = fopen("dummywrite","w");
    fwrite(buf, 1, 9, fp);
    fclose(fp);




    int *a = malloc(sizeof(int)*1000);
    int *b = malloc(sizeof(int)*1000); 
    
    
    printf("About to make syscall's  %d\n", (int) a);
      
    syscall(351,0,sizeof(int)*100,(int)a,15,1);
	syscall(351,1,sizeof(int)*100,(int)b,15,1);


    printf("Done with Syscall\n");

	return 1;
}
