#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int factorial(n, acc) {
	fork();
	if (n == 0) {
		printf("Result: %d\n", acc);
		return 0;
	}
	return factorial(n-1, acc*n);
}

int main()
{
	factorial(3, 1);
}