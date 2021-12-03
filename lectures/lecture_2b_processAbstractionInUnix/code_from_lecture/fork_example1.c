#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
	printf("I am ONE\n"); // prints ONCE
	fork(); // generates another CHILD process

	// at this point, there are 2 processes now: one PARENT and one CHILD
	printf("I am seeing DOUBLE\n"); // prints TWICE

	return 0;
}