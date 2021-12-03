#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

void SIGTSTP_handler(int sig);
void SIGINT_handler(int sig);
void SIGTERM_handler(int sig);

// int main()
// {
// 	pid_t child_pid = fork();

// 	if (child_pid == 0) {
// 		printf("--- CHILD process started --- \n");
// 		sleep(5);
// 		printf("--- CHILD process exiting --- \n");

// 		return 0;
// 	} else {
// 		int count = 0;

// 		while (count < 10) {
// 			int status;
// 			int any_changes = waitpid(child_pid, &status, WNOHANG);
// 			printf("any_changes = %d | WIFEXITED(status): %d | WIFSIGNALED(status): %d\n", any_changes, WIFEXITED(status), WIFSIGNALED(status));

// 			count++;
// 			sleep(1);
// 		}

// 		return 0;
// 	}
// }

/* ---------- SIGTERM ---------- */
// int main()
// {
// 	pid_t child_pid = fork();

// 	if (child_pid == 0) {
// 		signal(SIGTERM, SIGTERM_handler);

// 		printf("--- CHILD process started --- \n");
// 		sleep(5);
// 		printf("--- CHILD process exiting --- \n");

// 		return 0;
// 	} else {
// 		int count = 0;

// 		while (count < 10) {
// 			if (count == 5) {
// 				kill(child_pid, SIGTERM);
// 				printf("--- PARENT sent SIGTERM to CHILD process --- \n");
// 			}

// 			int status;
// 			int any_changes = waitpid(child_pid, &status, WNOHANG);
// 			printf("any_changes = %d | WIFEXITED(status): %d WEXITSTATUS(status): %d | WIFSIGNALED(status): %d WTERMSIG(status): %d\n", any_changes, WIFEXITED(status), WEXITSTATUS(status), WIFSIGNALED(status), WTERMSIG(status));

// 			count++;
// 			sleep(1);
// 		}

// 		return 0;
// 	}
// }

/* ---------- SIGTSTP ---------- */
int main()
{
	pid_t child_pid = fork();

	if (child_pid == 0) {
		signal(SIGTERM, SIGTERM_handler);

		printf("--- CHILD process started --- \n");
		sleep(5);
		printf("--- CHILD process exiting --- \n");

		return 0;
	} else {
		signal(SIGTSTP, SIGTSTP_handler);

		int count = 0;

		while (count < 10) {
			int status;
			int any_changes_WNOHANG = waitpid(child_pid, &status, WNOHANG);
			printf("\nany_changes_WNOHANG = %d\n WIFEXITED(status): %d WEXITSTATUS(status): %d \n WIFSIGNALED(status): %d WTERMSIG(status): %d\n", any_changes_WNOHANG, WIFEXITED(status), WEXITSTATUS(status), WIFSIGNALED(status), WTERMSIG(status));

			int any_changes_WUNTRACED = waitpid(child_pid, &status, WUNTRACED);
			printf("\nany_changes_WUNTRACED = %d\n WIFSTOPPED(status): %d WSTOPSIG(status): %d\n", any_changes_WUNTRACED, WIFSTOPPED(status), WSTOPSIG(status));

			count++;
			sleep(1);
		}

		return 0;
	}
}





/* ---------- SIGINT ---------- */





void SIGTERM_handler(int sig) {
	printf("caught\n");
}

void SIGTSTP_handler(int sig) {
	printf("\n SIGTSTP caught\n");
}