#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
  int child_pid = fork();

  if (child_pid == 0) {
    execl("/Users/jamesyeap1/Desktop/some_file", NULL);
  } else {
    int return_status = 0;
//    wait(&return_status);

    printf("%d\n", return_status);
  }
}
