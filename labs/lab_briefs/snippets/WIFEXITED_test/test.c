#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	pid_t child_pid = fork();

	if (child_pid == 0) {
		sleep(5);

		return 21;
	} else {
		int stat_loc;
		int timesQueried = 0;

		while (timesQueried <= 10) {
			int waitpid_returns = waitpid(child_pid, &stat_loc, WNOHANG);
			printf("waitpid returns: %d\n", waitpid_returns);
			printf("WIFEXITED returns: %d\n", WIFEXITED(stat_loc));
			printf("WEXITSTATUS returns: %d\n", WEXITSTATUS(stat_loc));
			printf("\n");


			sleep(1);
			timesQueried++;
		}
	}
}