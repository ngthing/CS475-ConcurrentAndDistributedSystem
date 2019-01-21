#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int counter = 0;
int main()
{
	int i;
	for (i = 0; i < 3; i ++){
	fork();
	counter ++;
	printf("counter = %d\n", counter);
	}
	printf("counter = %d\n", counter);
	return 0;
}