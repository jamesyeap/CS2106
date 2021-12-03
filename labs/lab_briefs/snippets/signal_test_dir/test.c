#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

void my_child_SIGINT_handler(int sig);
void my_parent_SIGINT_handler(int sig);
void my_SIGTSTP_handler(int sig);

int main()
{
  pid_t child_pid = fork();

   if (child_pid == 0) {
    signal(SIGINT, my_child_SIGINT_handler);

    int loops = 0;

    while (loops < 10) {
      printf("hello I am pid: %d...\n", getpid());

      sleep(1);
      loops+=1;
    }

    return 2;
   } else {
      printf("PARENT pid is: %d\n", getpid());
      signal(SIGINT, my_parent_SIGINT_handler);

      int child_stat;
      wait(&child_stat);

      if (WIFSIGNALED(child_stat)) {
          printf("\nCHILD signalled with: %d\n\nFor reference:\nSIGINT: %d\nSIGTSTP: %d\n\n", WTERMSIG(child_stat), SIGINT, SIGTSTP);

          if (WTERMSIG(child_stat) == SIGINT) {
            printf("\nCHILD received SIGINT\n");
            
            waitpid(child_pid, NULL, 0);
          }

          if (WTERMSIG(child_stat) == SIGTSTP) {
            printf("\nCHILD received SIGTSTP\n");
            
          } 
      }
   }
  
}

void my_child_SIGINT_handler(int sig) {
  // kill(getppid(), SIGINT);
}

void my_parent_SIGINT_handler(int sig) {
  printf("PARENT got SIGINT\n");
}


void my_SIGTSTP_handler (int sig) {
  printf("\nSIGTSTP caught in pid: %d\n", getpid());

  // kill(getpid(), SIGTSTP);
  // waitpid(getpid(), NULL, 0);
}
