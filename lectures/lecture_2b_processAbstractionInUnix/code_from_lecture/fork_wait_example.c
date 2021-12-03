#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int result, i, v = 10, status;
	for (i=0; i<5; i++) 
	{
			result = fork();
			if (result == 0)
			{
				// child
				printf("Child %i: exiting with %i\n", getpid(), v+i);
				exit(v+i);
			}
	}

	for (i=0; i<5; i++)
	{
		result = wait(&status);

		printf("Parent: found child %i with status %i\n", result, status);
	}
	printf("Parent says bye!\n");

	return 0;
}

/*

Child 10462: exiting with 10
Child 10463: exiting with 11
Child 10464: exiting with 12
Parent: found child 10463 with status 2816
Parent: found child 10462 with status 2560
Parent: found child 10464 with status 3072
Child 10465: exiting with 13
Parent: found child 10465 with status 3328
Child 10466: exiting with 14
Parent: found child 10466 with status 3584
Parent says bye!

*/