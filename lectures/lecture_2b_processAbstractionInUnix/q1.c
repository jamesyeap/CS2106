#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int result;

	result = fork();
	if (result == 0)
	{
		while(1) {
			sleep (2);
			wait(NULL);
		}
	}

	return 0;
}