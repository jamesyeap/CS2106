#include <stdio.h>
#include <unistd.h>

int main()
{
	printf("I am ONE\n");

	// Processes running at this point (n=1)
		//	1
	fork(); // <-- splits

	// Processes running at this point (n=2)
		//	1a 1b 
	fork(); // <-- splits

	// Processes running at this point (n=4)
	//	1a1 1a2 1b1 1b2
	fork(); // <-- splits

	// Processes running at this point (n=8)
	//	1a1a 1a1b 1a2a 1a2b 1b1a 1b1b 1b2a 1b2b

	printf("I am seeing DOUBLE\n"); // <-- this statement will thus be printed (n=8) times
	
	// note: the order in which these processes are printed will NOT be the same all the time
		//	NOT-DETERMINISTIC

	return 0;
}