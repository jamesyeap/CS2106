#include <stdio.h>
#include <unistd.h>

int main() 
{
	while (1) 
	{
		sleep(3);
		printf("I am still alive\n");
	}

	return 0; 
}