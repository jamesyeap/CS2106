#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>

int main()
{   
    int shmid, i, input, *shm;
    printf("Shared memory id for attachment: "); 
    scanf("%d", &shmid);
    
    shm = (int*)shmat( shmid, NULL, 0);
    if (shm == (int*)-1) {
        printf("Error: Cannot attach!\n");
        exit(1); 
    }    

    for (i=0; i<3; i++){
        scanf("%d", &input);        
        shm[i+1] = input;
    }
    shm[0] = 1;

    shmdt( (char*)shm );
    return 0; 
}