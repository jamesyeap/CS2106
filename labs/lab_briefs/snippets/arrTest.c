#include <stdio.h>

int main()
{
	//	a string is just an array of characters
	char some_string[] = "abc abc";
	char another_string[] = "def def";

	//	declare an array of pointer; these pointers point to arrays of characters
	//		in otherwords, this is an array of addresses of other arrays 
	char *arr_of_char_pointers[2];	//	<-- arr_of_char_pointers is a pointer to the first pointer in the array of pointers

	arr_of_char_pointers[0] = some_string;
	arr_of_char_pointers[1] = another_string;

	printf("%s\n", arr_of_char_pointers[0]);
	printf("%s\n", arr_of_char_pointers[1]);

	//	declare a pointer to the pointer
	char **tokens = arr_of_char_pointers;	//	<-- tokens is a pointer to the variable that holds the pointer to the first pointer in the array of pointers

	printf("%s\n", tokens[0]);
	printf("%s\n", tokens[1]);
}