#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
	//This is process P
	if ( fork() == 0 ){
		//This is process Q
		if ( fork() == 0 ) {
			//This is process R

			return 0;
		}
		/* <Point a> */
		// printf("point a\n");

		// return 0; // if R doesn't return
	}
	/* <Point b> */
	printf("point b\n");

	return 0;
}