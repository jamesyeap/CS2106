#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
  int i=-1;

  printf("i is: %d\n", WEXITSTATUS(i));

  return 0;
}
