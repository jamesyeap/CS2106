#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	pid_t child_pid = fork();

	if (child_pid == 0) {
		printf("pid: %d\n", child_pid);

		child_pid = fork();
		if (child_pid == 0) {
			printf("[inside child] pid: %d\n", child_pid);
		} else {
			printf("[inside child] pid: %d\n", child_pid);

			wait(NULL);
		}
	} else {
		printf("pid: %d\n", child_pid);

		wait(NULL);
	}
	
}