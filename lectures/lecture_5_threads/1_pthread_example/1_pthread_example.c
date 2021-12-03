#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int global_var;; 

void* do_sum(void* arg) {
	for (int i=0; i<1000; i++) {
		global_var++;
	}

	return NULL;
}

int main()
{
	pthread_t tid[5]; // can hold 5 thread IDs

	int i;

	global_var = 0;

	for (i=0; i<5; i++) {
		pthread_create(&tid[i], NULL, do_sum, NULL);
	}

	printf("Global variable is %i\n", global_var); // prints anything from 0 to 5000

	return 0;
}


