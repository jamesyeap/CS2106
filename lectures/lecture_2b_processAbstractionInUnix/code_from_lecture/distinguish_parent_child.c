#include <unistd.h>
#include <stdio.h>

int main()
{
	int result;
	result = fork();

	if (result != 0) 
	{
		printf("P:My Id is %i\n", getpid());
		printf("P:Child Id is %i\n", result);
	} else {
		printf("C:My Id is %i\n", getpid());
		printf("C:Parent Id is %i\n", getppid());
	}
}