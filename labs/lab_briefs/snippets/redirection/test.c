#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fcntl.h> //	import this for to use macros like "O_RDONLY", etc

int main()
{
	int fd_in = open("./a.txt", O_RDONLY);
	int fd_out = open("./b.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH);
	int fd_err = open("./c.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH);

	// printf("STDIN_FILENO: %d\nSTDOUT_FILENO: %d\nSTDERR_FILENO: %d\n", STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO);
	
	int fd_stdin = dup(STDIN_FILENO);
	dup2(fd_in, STDIN_FILENO); 

	int fd_stdout = dup(STDOUT_FILENO);
	dup2(fd_out, STDOUT_FILENO);

	int fd_stderr = dup(STDERR_FILENO);
	dup2(fd_err, STDERR_FILENO);

	pid_t child_pid = fork();

	if (child_pid == 0) {
		//	MAC OS DOESN'T HAVE /bin/sort !!! >:(
		// execl("/bin/sort", "/bin/sort", "-", NULL);

		execl("/bin/cat", "/bin/cat", NULL);

		//	use _exit instead of exit to catch possible exec error; 
		//		_exit() only performs kernel cleanup while exit() cleans up user-mode constructs
		//		(don't quite understand this tbh)
		_exit(1); 
	} else {
		int exit_status;
		wait(&exit_status);

		/* RESET stdin, stdout, stderr back to original inputs */
		dup2(fd_stdin, STDIN_FILENO);
		dup2(fd_stdout, STDOUT_FILENO);
		dup2(fd_stderr, STDERR_FILENO);

		printf("exit_status: %d\n", exit_status);
	}
}
