#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>
#include <inttypes.h>

#define PAGESIZE 4096

int v = 5; // in data-region

int main()
{
	// initialise a SHARED (must specify MAP_SHARED) memory-region
	uint32_t *shared_memory = mmap(NULL, PAGESIZE, 
		PROT_READ | PROT_WRITE, 
		MAP_SHARED | MAP_ANONYMOUS, 
		-1, 0
	);

	pid_t pid; pid = fork();

	if (pid == 0) {	/* CHILD process */
		v = 10; 				// child modifies data-variable, but parent won't see
		*shared_memory = 15;	// child modifies SHARED memory-mapping -> seen by parent too!

		// printf("%lu\n", (unsigned long)shared_memory);
	} else {		/* PARENT process */
		wait(NULL); // wait for child to finish

		printf("v (not shared): %d\n", v);
		printf("shared_memory (shared): %d\n", *shared_memory);

		// printf("%lu\n", (unsigned long)shared_memory);
	}
}