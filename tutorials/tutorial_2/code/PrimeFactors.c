#include <stdio.h>
#include <stdlib.h>

//Give the total number of prime factors
// not unique prime factors :-)
// e.g. 9 = 3 x 3  (2 prime factors)

//No checks on argc, assume the user pass in the right number of
// arguments :-)

int main( int argc, char* argv[])
{
    int nFactor = 0, userInput, factor;
    if (argc < 2)
    {
        printf ("Expected command: %s <value_to_be_factorized>\n", argv[0]);
        return 0;
    }

    //Convert string to number
    userInput = atoi( argv[1] );

    nFactor = 0;
    factor = 2;

    // quick hack to get the number of prime factors
    // only for positive integer
    while (userInput > 1)
	{
        if (userInput % factor == 0)
		{
            userInput /= factor;
            nFactor++;
        } else {
            factor++;
        }
    }

    return nFactor;
}
