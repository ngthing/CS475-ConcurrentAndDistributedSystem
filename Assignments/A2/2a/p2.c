#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

pid_t pid;
int counter = 0;
void handler1(int sig)
{
	counter--;
	printf("counter = %d\n", counter);
	fflush(stdout); /* Flushes the printed string to stdout */
	kill(pid, SIGUSR1);
}
void handler2(int sig)
{
	counter += 2;
	printf("counter = %d\n", counter);
	exit(0);
}
main() {
	signal(SIGUSR1, handler1);
	if ((pid = fork()) == 0) {
	signal(SIGUSR1, handler2);
	kill(getppid(), SIGUSR1);
	while(1) {};
	}
	else {
	pid_t p; int status;
	if ((p = wait(&status)) > 0) {
		counter += 3;
		printf("counter = %d\n", counter);
		}
	}
}