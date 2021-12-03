#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void my_SIGSEGV_handler(int sig) {
	if (sig == SIGSEGV) {
		printf("Memory access blows up\n");
		
		exit(1);
	}
}

int main()
{
	int *ip = NULL; // a pointer to NOWHERE

	if (signal(SIGSEGV, my_SIGSEGV_handler) == SIG_ERR) {
		printf("Failed to register custom handler\n");
	}

	*ip = 123; // this assigns '123' to... you guessed it, NOWHERE

	return 0;
}