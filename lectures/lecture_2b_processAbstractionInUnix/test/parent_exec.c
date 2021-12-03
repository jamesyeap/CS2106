#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
	int childResult = 5;

	int pid = fork();
	char *the_path = "/Users/jamesyeap1/Desktop/CS2106/lectures/lecture_2b/test/some_code";

	if (pid == 0) {
		// exit(0);
		execl(the_path, NULL);
	} else {
		printf("childResult: %d\n", childResult);
		int childPid = wait( &childResult );
		printf("childResult: %d\n", childResult);
	}
}