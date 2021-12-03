#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	pid_t pid = fork();

	if (pid != 0) { // parent sleeps
		printf("parent pid: %d\n", getpid());

		sleep(30);
	} else { // child just exits -> becomes a zombie		
		printf("child pid: %d\n", getpid());
	}
}