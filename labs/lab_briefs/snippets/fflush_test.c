#include <stdio.h>

int main (void)
{
	char A,B,C;

	scanf("%c", &A);
	fflush(stdin); // need fflush to stop the terminal from putting "enter" key into B

	scanf("%c", &B);
	fflush(stdin); // need fflush to stop the terminal from putting "enter" key into C

	scanf("%c", &C);

	//	WITHOUT fflush (wrong)
		/* inputForA, enterKey, inputForB */
		/* 		A 		 B 			C     */

	//	WITH fflush (correct)
		/* inputForA, inputForB, inputForC */
		/* 		A 		 B 			C     */


	printf("%c %c %c\n", A,B,C);
}