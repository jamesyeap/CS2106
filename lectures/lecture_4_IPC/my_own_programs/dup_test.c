// OBJECTIVE: Show that when dup() duplicates a file-descriptor, this new file-descriptor points to
//		the SAME File-Entry in the "System-wide Open File Table" as the file-descriptor that was duplicated.

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main() {
	int fd1 = open("a.txt", O_RDWR | O_CREAT);

	// prepare first set of data
	char *data1 = malloc(5 * sizeof(int));
	for (int i=0; i<5; i++) {
		data1[i] = 'a';
	}

	// write data1 into "a.txt"
	write(fd1, data1, 5);

	// then, duplicate the file-descriptor
	int fd2 = dup(fd1);

	// prepare second set of data
	char *data2 = malloc(5 * sizeof(int));
	for (int i=0; i<5; i++) {
		data2[i] = 'b';
	}

	// write data2 into "a.txt" using the NEW file-descriptor
	write(fd2, data2, 5);

}