#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>

int main() 
{
    int shmid, i, *shm;
    shmid = shmget( IPC_PRIVATE, 40, IPC_CREAT | 0600); 

    if (shmid == -1){
        printf("Cannot create shared memory!\n"); exit(1);
    } else
        printf("Shared Memory Id = %d\n", shmid);

        shm = (int*) shmat( shmid, NULL, 0 ); 
        if (shm == (int*) -1){
            printf("Cannot attach shared memory!\n");
            exit(1); 
        }

        shm[0] = 0;
        while (shm[0] == 0){ 
            sleep(3);
        }

        for (i = 0; i < 3; i++){
            printf("Read %d from shared memory.\n", shm[i+1]);
        }

        shmdt( (char*) shm);
        shmctl( shmid, IPC_RMID, 0); 

        return 0;
}