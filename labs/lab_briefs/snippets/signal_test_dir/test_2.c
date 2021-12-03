#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

void PARENT_SIGINT_handler(int sig);
void PARENT_SIGTSTP_handler(int sig);
void CHILD_SIGINT_handler(int sig);
void CHILD_SIGTSTP_handler(int sig);

int main()
{

	int child_pid = fork();
	if (child_pid == 0) {
		// signal(SIGINT, CHILD_SIGINT_handler);
		// signal(SIGTSTP, CHILD_SIGTSTP_handler);

		sleep(10);

		return 0;
	} else {
		signal(SIGINT, PARENT_SIGINT_handler);
		signal(SIGTSTP, PARENT_SIGTSTP_handler);

		int stat;
		waitpid(child_pid, &stat, WUNTRACED);

		if (WIFSTOPPED(stat) && WSTOPSIG(stat) == SIGTSTP) {
			printf("\nChild received SIGTSTP\n");

			return 2;
		}

		if (WIFSIGNALED(stat) && WTERMSIG(stat) == SIGINT) {
			printf("\nChild received SIGINT\n");

			return 1;
		}


		return 0;
	}
}

void PARENT_SIGINT_handler(int sig) {
	// do nothing; just prevent PARENT from actually being interrupted
	printf("PARENT recevied SIGINT\n");
}

void CHILD_SIGINT_handler(int sig) {
	// nothing here for now
	printf("CHILD received SIGINT\n");
}

void PARENT_SIGTSTP_handler(int sig) {
	// do nothing; just prevent PARENT from actually stopping
	// printf("PARENT recevied SIGTSTP\n");
}

void CHILD_SIGTSTP_handler(int sig) {
	// printf("CHILD recevied SIGTSTP\n");
}