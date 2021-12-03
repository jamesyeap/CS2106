#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

struct pipelock {
    int pipe_fd[2];
};

void lock_init(struct pipelock *lock);
void lock_acquire(struct pipelock *lock);
void lock_release(struct pipelock *lock);

int globalVar = 1; 

void* foo(void *lock) {
    lock_acquire(lock);

    int localVar = globalVar;
    localVar++;

    usleep(100); // sleeps for 0.1 seconds

    globalVar = localVar; // increment globalVar by 1
    printf("%d\n", globalVar);

    lock_release(lock);

    return 0;
}

int main()
{

    struct pipelock *lock = malloc(sizeof(struct pipelock));
    lock_init(lock);

    pthread_t id[5];
    for (int i=0; i<5; i++) {
        pthread_create(&id[i], NULL, foo, lock);
    }

    for (int i=0; i<5; i++) {
        pthread_join(id[i], NULL);
    }

    printf("final value of globalVar: %d\n", globalVar);
}

void lock_init(struct pipelock *lock) {
    pipe(lock->pipe_fd);    

    int fakeBuffer[] = {1};
    write(lock->pipe_fd[1], fakeBuffer, 1);
}

void lock_acquire(struct pipelock *lock) {
    int fakeBuffer[1];
    read(lock->pipe_fd[0], fakeBuffer, 1);
}

void lock_release(struct pipelock *lock) {
    int fakeBuffer[] = {1};
    write(lock->pipe_fd[1], fakeBuffer, 1);
}