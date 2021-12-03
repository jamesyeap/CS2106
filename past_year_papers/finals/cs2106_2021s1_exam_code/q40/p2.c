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
	int *ptr;
	scanf("%p", &ptr);

	printf("%p\n", ptr);

	// initialise a SHARED (must specify MAP_SHARED) memory-region
	uint32_t *shared_memory = mmap(ptr, 0, 
		PROT_READ | PROT_WRITE, 
		MAP_SHARED | MAP_ANONYMOUS, 
		-1, 0
	);

	printf("%p\n", shared_memory);

	// *shared_memory = 5;


}