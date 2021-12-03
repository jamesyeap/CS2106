#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define READ_END 0
#define WRITE_END 1

int main() 
{
     int pipe_fd[2]; // allocate array for the file-descriptors of the pipe
     pipe(pipe_fd); // create a new pipe, and assign the file-descriptors of the two ends to the pipe_fd array

     pid_t child_pid = fork();
     if (child_pid != 0) { // parent process
          char str[] = "Hello there!";

          // close(pipe_fd[READ_END]);
          write(pipe_fd[WRITE_END], str, strlen(str)+1);
          // close(pipe_fd[WRITE_END]);

     } else { // child process
          char buffer[100]; // data will be written into this buffer

          // close(pipe_fd[WRITE_END]);
          int len = read(pipe_fd[READ_END], buffer, sizeof(buffer));
          printf("Proc %d read: %s\n", getpid(), buffer);
          // close(pipe_fd[READ_END]);
     }

}