# Process Abstraction

## 3 main Execution Contexts
- `Hardware` Context
- `Memory` Context
- `OS` Context

## `Hardware` Context
- `GPR`
- `PC`
- Stack Pointer `SP`
- Stack Frame Pointer `FP`
- ...

## `Memory` Context
`Memory` space generally has 4 regions:
- Text (Instructions)
- Data (Global Variables)
- Heap (for Dynamic Allocation)
- Stack (for Function Calls)

## Function Call
### Issues
- Control Flow Issues
	- TODO
- Data Storage Issues
	- TODO
- Solution
	- Allocate a new region in `Memory` called the `Stack` to be used for Function Calls (aka Function Invocations)

## Stack
- `Stack` grows "upwards" in `Memory` - [illustration in Slide 16]
	- unlike `Heap`, which grows "downwards"
- Stack Pointer `SP` points to a particular point in `Memory`
	- points to the top of the `Stack` region of the `Memory`
		- which is the first unused location in `Stack`
- For every Function Call made, a new `Stack Frame` is pushed on top of the current `Stack`
	- For recursive Function Calls, a new `Stack Frame` is pushed every time the Function is recursed.

### Stack Frame
A `Stack Frame` contains:
- Return address of the caller (aka Return `PC`)
- Arguments (Parameters) for the function
- Storage for local variables
- Other info...
- [illustration in Slide 22]

## Stack Frame Setup and Teardown
- 2 entities
	- Caller
	- Callee

- 2 stages
	- On executing Function Call -> Stack Frame Setup
	- On returning from Function Call -> Stack Frame Teardown

Setting up a `Stack Frame` (example only; actual implementation may vary)
- Caller 
	- Pass parameters using either
		- GPR or,
		- `Stack`
	- Save Return PC on `Stack`

After setting up part of the `Stack Frame`, the Caller will jump to the location of the instruction of the Callee (Transfer of Control).

- Callee
	- Save the old Stack Pointer `SP`
		- required to "pop" off the current `Stack Frame` once this Callee function is done executing
	- Save the old Frame Pointer `FP`
	- Allocate space for local variables of the callee on the `Stack`
	- Adjust the Stack Point `SP` to point to the top of the new `Stack` region

Tearing down a `Stack Frame` (example only; actual implementation may vary)
- Callee
	- Restore the saved registers
	- Restore the old Frame Pointer `FP`
	- Restore the old Stack Pointer `SP`

After performing these teardown actions, the Callee will jump to the location of the instruction of the Caller using the saved `PC` (Transfer of Control).

- Caller
	- Continues executing.

- note: A `Stack Frame` is not always wiped off the `Memory` after the Function Call
	- implication: it's very important to initialize a new variable (e.g int i = 0)
		- if not: (int i), because it's uninitialized, may contain a "garbage" value left behind by another `Stack Frame`

	- this behaviour depends on the Compiler

### Frame Pointer `FP`
- Frame Pointer `FP` points to a fixed location in the `Stack Frame`
- why need `FP`? 
	- Stack Pointer `SP` can change
	- more convincing reasons: TODO
- note: the usage

### Saved Registers
- Register-Spilling
	- Sometimes, not enough `GPR`s to store all the data used.
		- MIPS has 32 GPRs, x86 has 16 `GPR`s
	- Solution:
		- Use `Memory` to temporarily hold the value of the `GPR`s of the current Function Call.
			- Then, the `GPR` can be used to store other values for another Function Call.
		- When the other Function Call is done, the values of the `GPR`s of the current Function Call can be restored, and continue running as normal.

#### Calling Convention (2 types)
### Callee-Saved 
	- The Callee-Function performs Register-Spilling before the function-body has executed.
	- Restore these registers to prior values after the function-body has completed

### Caller-Saved
... TODO

## Dynamically Allocated Memory (`Heap`)
`Heap` grows "downwards" in `Memory` - [illustration in Slide 37]
- unlike `Stack`, which grows "upwards"

Reasons why it's needed:
- TODO

A lot more trickier to manage (as compared to `Stack`), due to issues like:
- Size of Variable
- Timing of Allocation and Deallocation of Variables
- Example: "Holes" will be created:
	- AABBCCDD
	- AA__CCDD
	- question: what if you have EEE? cannot fit inside the hole right cuz it's too big (the gap only has 2 spaces)
		- so, it's an issue.
		- possible solutions are covered much later in this course.

___

# Introduction to `Process` Management

## `OS` Context
- `Process` Properties
	- Process ID (`PID`)
		- Unique identifier
		- Some OS-dependent issues:
			- are `PID`s reused?
			- any reserved `PID`s?
			- any limit to the number of `Process` instances due to the max number of `PID`s possible? (fixed-width issue)
	- Process `State`
		- Generic "5-State Process Model"
			- `New` (may still be initializing)
			- `Ready` (waiting to run on the CPU)
			- `Running` (being executed on the CPU)
			- `Blocked` (waiting for an event to be available - cannot execute until then)
			- `Terminated` (finished execution, may require OS cleanup)

Process State Transitions
- TODO 

Queuing Model of "5-State Process Model"
- Ready Queue
	- TODO
- Blocked Queue
	- TODO

Assumption in this course: CPU only has 1 Core

## Process Control Block (`PCB`) aka Process Table Entry (`PTE`)
Stores the entire Execution Context for a `Process`
- `Hardware` Context
- `Memory` Context
- `OS` Context

`Kernel` maintains the PCB for all `Process`
- Conceptually, you can think of this as a Table or a Stack
	- "although it doesn't have to be a Table or a Stack" (said during Lecture 2, around 1hr40min)

refer to Lecture 2, Slide 58 for illustration of `PCB`

## UNIX System Calls
- NOT the same as a normal Function Call
	- have to change from `User Mode` to `Kernel Mode`
- Most UNIX variants follow POSIX standards
	- Windows variants do not.

## System Calls in C Programs
System Calls can be invoked almost directly using the `Library`, which has 2 flavours of Calls:
### Function Wrapper

### Function Adapter (more user-friendly)

## System Call Mechanism 
### Stage 1 (in `User Mode`)

TRAP instruction 
- to switch to `Kernel Mode`

### Stage 2 (in `Kernel Mode`)

### Stage 3 (in `User Mode`)

## Exceptions

- Exceptions are Synchronous

### Exit Status
```C
#include <stdio.h>

int main()
{
	int var = 10/0; // ERROR
	printf("All is well."); // this will not be printed

	return 0; // exit code is NOT 0
}
```
```bash
# Prints the exit code of the last function call 
echo $? # prints 0 if last function call executes without any exceptions
		#	prints some other code if some exception was encountered
```

## Interrupt

### Manual Interrupt (3 Keyboard Commands)
`CTRL + Z`: Suspends the process
```bash
# To resume the process, type "fg <process_number>"
fg 1
```

`CTRL + C`: Terminates the process

`CTRL + D`: End-Of-File
- closes the stdin pipe
- don't quite understand: TODO















