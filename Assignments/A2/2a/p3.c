#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

int counter = 0;
void handler(int sig)
{
	counter = counter + 3;
}
int main()
{
	int i;
	signal(SIGCHLD, handler);
	for (i = 0; i < 5; i ++){
	if (fork() == 0){
		exit(0);
		}
	}
	/* wait for all children to die */
	while (wait(NULL) != -1);
	printf("counter = %d\n", counter);
	return 0;
}