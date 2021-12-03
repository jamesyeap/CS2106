#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

void my_SIGINT_handler(int sig);
void my_SIGTSTP_handler(int sig);

int main()
{


  pid_t child_pid = fork();

   if (child_pid == 0) {
    // signal(SIGINT, my_SIGINT_handler);
    printf("CHILD pid is: %d\n", getpid());

    int loops = 0;

    while (loops < 10) {
      printf("hello I am pid: %d...\n", getpid());
      sleep(1);
      loops+=1;
    }

    return 2;
   } else {
      printf("PARENT pid is: %d\n", getpid());
      signal(SIGINT, my_SIGINT_handler);
      signal(SIGTSTP, my_SIGTSTP_handler);

      int child_stat;
      wait(&child_stat);

      if (WIFSIGNALED(child_stat)) {
          printf("PARENT signalled with: %d\nSIGINT: %d\nSIGTSTP: %d\n", WIFSIGNALED(child_stat), SIGINT, SIGTSTP);
      }

   }
  
}

void my_SIGINT_handler(int sig) {
  printf("SIGINT caught in pid: %d\n", getpid());
}


void my_SIGTSTP_handler (int sig) {
  printf("SIGTSTP caught in pid: %d\n", getpid());
}
