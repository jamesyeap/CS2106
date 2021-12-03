#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	int stat;

	int child_pid = fork();
	if (child_pid == 0) {
		printf("child's PID is: %d\n", getpid());

		// execl("./routine", "./routine", NULL);
		// execl("./problematic_routine", "./problematic_routine", NULL);
		execl("/bin/sort", "/bin/sort", "a.txt", NULL);

	} else {
		int what_child = wait(&stat);

		//	WIFEXITED: This macro returns a nonzero value if the child process terminated normally with exit or _exit.
		printf("WIFEXITED gives: %d\n", WIFEXITED(stat));

		printf("child with pid: %d exited with exit status of: %d\n", what_child, WEXITSTATUS(stat));
	}
}