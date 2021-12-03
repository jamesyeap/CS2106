#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

void* open_file(void *arg) {
	int fd = open("a.txt", O_RDONLY);

	printf("fd: %d\n", fd);

	return NULL;
}

int main() {
	pthread_t tid[5];

	for (int i=0; i<5; i++) {
		pthread_create(&tid[i], NULL, open_file, NULL);
	}

	//	wait for all threads to finish
	for (int i=0; i<5; i++) {
		pthread_join(tid[i], NULL);
	}

	int fd = open("a.txt", O_RDONLY); printf("fd: %d\n", fd);

	char *buffer = malloc(sizeof(char) * 100);

	read(3, buffer, 11); // can put any integer from 3 up to 9 and "hello world" would still be printed
	printf("%s\n", buffer);

	printf("DONE\n");
}

