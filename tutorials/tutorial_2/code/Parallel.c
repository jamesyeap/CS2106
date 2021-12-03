#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
int main()
{
    int userInput, childPid, childResult;
    //Since largest number is 10 digits, a 12 characters string is more
    //than enough
    char cStringExample[12];

    scanf("%d", &userInput);

    childPid = fork();

    if (childPid != 0 )
	{
        wait( &childResult );
        printf("%d has %d prime factors\n", userInput, childResult >> 8);

    } else {
        //Easy way to convert a number into a string
        sprintf(cStringExample, "%d", userInput);

        execl("./PF", "PF", cStringExample, NULL);
    }

}
*/

int main()
{
    int numberOfInputs, childResult, completedPid;

    //  Since largest number is 10 digits, a 12 characters string is more
    //  than enough
    char cStringExample[12];

    // read in how many inputs are to be processed
    scanf("%d", &numberOfInputs);

    int userInputs[numberOfInputs];
    int childPid[numberOfInputs];

    // create a new child process for each input
    for (int i=0; i<numberOfInputs; i++)
    {
        // read in the inputs and store it inside an array
        scanf("%d", &userInputs[i]); 

        // store the PID of the CHILD process inside an array
        childPid[i] = fork(); 

        if (childPid[i] == 0) { 
            /* ---------- In the CHILD process ---------- */

            // convert the input into a string
            sprintf(cStringExample, "%d", userInputs[i]);
            // run PrimeFactorial with the stringified input
            execl("./PF", "abcdsdadadawwa", cStringExample, NULL); // <-- second argument of execl "somenonsensething" not actually used
        } else {
            /* ---------- In the PARENT process ---------- */            
            
            // do nothing
        }
    }

    //  Now, you need to wait for numberOfInputs of processes to complete 
    //      Once a process is done, you want to print the result.
    for (int i=0; i<numberOfInputs; i++) {
        //  this line will run when ANY of the CHILD processes have completed
        //      BUT, how do you know WHICH one of them has completed? 
        //      this matters because you also need to print in the input, together with the result
        completedPid = wait( &childResult );

        // find the input that corresponds to the result
        for (int j=0; j<numberOfInputs; j++) {
            if (completedPid == childPid[j]) {
                printf("%d has %d prime factors\n", userInputs[j], childResult >> 8);                
            }
        }
    }
    
}
