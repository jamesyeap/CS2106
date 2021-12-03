#include <stdio.h>
#include <unistd.h>

int main()
{
	int pid = fork();
	if (pid == 0) {
		execl("/bin/ls", "ls", "-l", NULL);
	}
	printf("All good!\n");
}