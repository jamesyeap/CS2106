#include <unistd.h>
#include <stdio.h>

int main() 
{
	int var = 1234; 
	int result;
	result = fork(); // <-- if "Parent" process (= PID of "Child" Process); if "Child" process (= 0)

	if (result != 0) 
	{
		printf("Parent: Var is %i\n", var); // 1234
		var++;
		printf("Parent: Var is %i\n", var); // 1235 <-- notice that it's not affected by the "Child" process modification
	} else {
		printf("Child: Var is %i\n", var); // 1234 
		var--;
		printf("Child: Var is %i\n", var); // 1233 <-- notice that it's not affected by the "Parent" process modification
	}
}