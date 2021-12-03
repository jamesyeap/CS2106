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

	//	wait for all threads to finish before moving onto the next line
	for (i=0; i<5; i++) {
		pthread_join(tid[i], NULL);
	}

	//	this line only executes AFTER all the threads have completed
	printf("Global variable is %i\n", global_var); // prints 5000 everytime now
													//	** eh actually no leh **

	return 0;
}