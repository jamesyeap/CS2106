#include <stdio.h>
#include <unistd.h>

int main() {
  int i;
  for (i = 0; i < 3; i++){
    fork();
    printf("Hello World!\n");
    execl("./Echo", "Echo",
                   "Hello World!");
    printf("Hello World!\n");
  }
  return 0;
}