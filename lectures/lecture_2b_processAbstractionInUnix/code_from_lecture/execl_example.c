#include <stdio.h>
#include <unistd.h>

int main()
{
	execl("/bin/s", "ls", "-al", NULL);

	printf("Am I still here?\n"); // <-- lines after "execl" not executed usually! UNLESS "execl" has problematic execution.
	return 0;
}