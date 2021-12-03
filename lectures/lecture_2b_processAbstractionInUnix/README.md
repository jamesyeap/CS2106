# Process Abstraction in Unix

# Status of Processes
```bash
# shows status of your processes
ps

# shows everyone's (not only yours) process
ps -ef

# configure what info you want to see about your processes
ps -o pid,ppid # only shows 2 columns -> "pid" and "ppid"

# get more info about a command
man ps # shows info about for the command "ps" such as optiona
```
# Create a new Process
```C
#include <unistd.h>

int main()
{
	// ... body statements
	int fork();
}
```
`fork()`
- returns `PID` of the newly-created process if the program executing is the `Parent`.
- returns `0` if the program executing is the `Child`.

## Distinguish between `Parent` and `Child` process
Use the return value of `fork()` to distinguish parent and child.
- `getpid()`: get current Process ID
- `getppid()`: get Process ID of `Parent`

```C
#include <unistd.h>
#include <stdio.h>

int main()
{
	int result;
	result = fork();

	if (result != 0) 
	{
		printf("P:My Id is %i\n", getpid());
		printf("P:Child Id is %i\n", result);
	} else {
		printf("C:My Id is %i\n", getpid());
		printf("C:Parent Id is %i\n", getppid()); // <-- may not print the correct Parent's PID here
													// this happens if the Parent process has Terminated
														// before the Child process can run 
														// (see: Orphan process)
	}
}
```

## `Orphan` process
If the `Parent` process is scheduled to run before the `Child` process, the `Parent` process may Terminate before the `Child` process can run. 

### What's printed in `ppid()` now?
How UNIX handles this is that the `init` process will Inherit these `Orphan` processes
- thus, calling `ppid()` in the `Orphan` process will no longer print the `Parent`'s PID, but that of (TODO: ...)

#### `init` process
- created in kernel at boot-up time
- `init` process traditionally has a PID = 1
- `init` is the root process of the process-tree created by `fork()`

## `Parent` and `Child` processes have separate Address-Spaces
```C
#include <unistd.h>
#include <stdio.h>

int main() 
{
	int var = 1234; 
	int result;
	result = fork(); // <-- if "Parent" process (= PID of "Child" Process); if "Child" process (= 0)

	if (result != 0) 
	{
		printf("Parent: Var is %i\n", var);
		var++;
		printf("Parent: Var is %i\n", var);
	} else {
		printf("Child: Var is %i\n", var);
		var--;
		printf("Child: Var is %i\n", var);
	}
}
```
In the above example, note that even though the variable `var` is modified by the `Parent` process, the variable `var` referenced in the `Child` process WILL STILL BE THE SAME.
- this is true for data stored on both the Stack and the Heap
- can think of it as a COPY of the memory context is created for the `Child` process

# Executing a New Program/Image 
`fork()` is not very useful because... (TODO)

`execl()`

## Characteristic of `execl()`
```C
#include <stdio.h>
#include <unistd.h>

int main()
{
	execl("/bin/s", "ls", "-al", NULL); // <-- "replaces" the current process with the process specified here

	printf("Am I still here?\n"); // <-- NOT PRINTED IN MOST CASES! only printed if process above FAILS
	return 0;
}
```
All lines after the `execl()` command are usually not executed 
	- UNLESS the proces specified in `execl()` fails.
		- only in this scenario, then the lines below `execl()` will continue executing.

# Command Line Argument in C
... TODO

# Process Termination in UNIX (`exit()`)
note: the Function does not return!


## Becoming a `Zombie` process
On Process Exit, only MOST of the resources are released
- cannot delete ALL info about the Process
	- what if the Process is a `Child` and the `Parent` needs some info such as its "PID"?
		- thus, can only be cleaned uyp when `wait()` happens
	- When a `exit()` is called in a program, the Process that is running it may become a `Zombie` process
		- if `Parent` process does not call `wait()` on it

Return from `main()` implicitly calls `exit()`

Example 1
```C
/* void exit( int status ); */

#include <stdlib.h>

int main()
{
	printf("harlo world");
	exit(0); // <-- to indicate that the process executed successfully

	// return 0 <-- return is not used here
}
```

Example 2
```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int result, i, v = 10, status;

	for (i=0; i<5; i++) 
	{
			result = fork();
			if (result == 0)
			{
				// child
				printf("Child %i: exiting with %i\n", getpid(), v+i);
				exit(v+i);
			}
	}

	for (i=0; i<5; i++) // <-- Parent processes waits for five Child processes
	{
		// parent
		result = wait(&status); // <-- Parent waits for one Child process to terminate
		printf("Parent: found child %i with status %i\n", result, status);
	}
	printf("Parent says bye!\n"); // <-- when all five Child processes are done executing

	return 0;
}

// OUTPUT
/*

Child 10462: exiting with 10
Child 10463: exiting with 11
Child 10464: exiting with 12
Parent: found child 10463 with status 2816
Parent: found child 10462 with status 2560
Parent: found child 10464 with status 3072
Child 10465: exiting with 13
Parent: found child 10465 with status 3328
Child 10466: exiting with 14
Parent: found child 10466 with status 3584
Parent says bye!

*/
```


## UNIX Convention
Normal termination: `exit(0)`
Problematic executation: `exit(!0)` 

# `Parent` and `Child` Synchronisation in UNIX (`wait()`)
`Parent` process can wait for the `Child` process to terminate before continuing.
```C
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
	int child_pid, child_exit_code;
	printf("Hello");

	child_pid = fork();

	// "wait()" stores the exit status of the Child process in the argument given 
	wait(&child_exit_code); // <-- Parent process is Blocked here until at least one Child terminates
	// wait(NULL); <-- use NULL if you don't need the exit status of the Child process

	if (child_exit_code == 0) 
	{
		printf("Child process executed successfully, exiting now...");
	} else {
		printf("Child process faced error.")
	}
}
```

## `Zombie` process
`Child` process terminates before `Parent` process AND `Parent` process doesn't call `wait()` to cleanup
`Child` process -> `Zombie` process.

### Handling of `Zombie` process
Can fill up the Process Table
- may need a reboot to clear the Process Table on older UNIX implementations

## `Orphan` process
`Parent` process terminates before `Child` process.
`Child` process -> `Orphan` process.

### Handling of `Orphan` process
`init` process will inherit this `Orphan` process
- becoming its "psuedo" parent (thus calling `ppid()` inside the `Orphan` process will print the PID of the `init` process, which is usally PID=1)

After the `Orphan` process terminates, a signal is sent to `init` process
- which then uses `wait()` to cleanup  

`wait()` can be used by the `Parent` process to terminate `Zombie process`

## Variations of `wait()`
`waitpid()`: wait for a SPECIFIC `Child` process
`waitid()`: wait for any `CHILD` process to CHANGE STATUS



