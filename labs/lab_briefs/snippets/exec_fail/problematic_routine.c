#include <stdio.h>
#include <stdlib.h>

int main()
{

	int confirm_error = 1 / 0;
	printf("EXIT_FAILURE is a MACRO representing the integer: %d\n", EXIT_FAILURE);
	exit(EXIT_FAILURE);

	// int exit_code = 1;

	// printf("something went wrong, RETURNING with code: %d\n", exit_code);
	// return exit_code;

	// printf("something went wrong, EXITING with code: %d\n", exit_code);
	// exit(exit_code);
}