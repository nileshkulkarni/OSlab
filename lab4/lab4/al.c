#include <signal.h>

void watchdog(int sig) 
{
  printf("Pet the dog\r\n");
  /* reset the timer so we get called again in 5 seconds */
  alarm(1);
}


/* start the timer - we want to wake up in 5 seconds */
int main()
{
  /* set up our signal handler to catch SIGALRM */
  signal(SIGALRM, watchdog);
  alarm(1);
  while (1) 
   ;
}
