#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char *argv[])
{
    int i, shmid, n = 100, childPid, nChild = 1, pOrder = 0;
    int *shm;

    if (argc > 1)
        n = atoi(argv[1]);

    if (argc > 2)
        nChild = atoi(argv[2]);

    // create Share Memory Region
    // read, write, excute
    // 1      1      0        = 6
    // 1      0      0        = 4
    
    // TODO: create a shared memory region that contains two values

    // TODO: attach the shared memory region to this process

    // TODO: initialize the shared memory

    for (i = 0; i < nChild; i++) {
        childPid = fork();
        if (childPid == 0) {
            pOrder = i + 1; // each process gets its pOrder
            break;
        }
    }

    // TODO: only increment the shared value if it's the process' turn
    // don't forget to let the other process know its turn has come

    if (childPid != 0) {
        for (i = 0; i < nChild; i++)
            wait(NULL);
        // TODO: print the result value
        // TODO: detach and destroy
    }

    return 0;
}
