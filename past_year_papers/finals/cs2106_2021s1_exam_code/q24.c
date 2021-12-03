#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int t = 0;
void* thread(void* i)
{
	int *s;
	s = (int *)i;
	(*s)++;

	return NULL;
}

int main()
{
	pthread_t tid[102]; // can hold 100 thread IDs

	for (int i=0; i<100; i++) {
		pthread_create(&tid[i], NULL, thread, NULL);
	}

	t++;

	// printf("%d", t);
}