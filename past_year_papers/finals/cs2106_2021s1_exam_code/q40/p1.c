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

	printf("%p\n", shared_memory);

	sleep(15);

	printf("%d\n", *shared_memory);
}