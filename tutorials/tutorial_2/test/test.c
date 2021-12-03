#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	if (fork() == 0) 
	{
		printf("I am Q, and I am the child of P\n");	
		if (fork() == 0)
		{
			printf("I am R, and I am the child of Q\n");

			return 0;
		} else {
			wait(NULL);
		}
	} else {
		sleep(1);
		// wait(NULL);
		printf("I am P\n");
	}
}