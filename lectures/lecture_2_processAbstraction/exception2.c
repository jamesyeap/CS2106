#include <stdio.h>

int main()
{
	int* p = NULL;
	*p = 100; // ERROR HERE

	printf("Never printed");
	return 0;
}