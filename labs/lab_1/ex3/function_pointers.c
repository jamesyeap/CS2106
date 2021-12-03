/*************************************
* Lab 1 Exercise 3
* Name: Yeap Yi Sheng James
* Student No: A0218234L
* Lab Group: B09
*************************************/

#include "functions.h"

// Initialize the func_list array here!
/* Fill this up */;
int (*func_list[5])(int) = { &add_one, &add_two, &multiply_five, &square, &cube };

// You can also use this function to help you with
// the initialization. This will be called in ex3.c.
void update_functions() {
	// The runner should be easily extensible to allow for 
	// 		new operations for the map function without 
	// 		changing the implementation of ex3.c and node.c.

	// Adding or removing a new MAP operation should be done 
	// 		by modifying only the array of function pointers 
	//		in function_pointers.c and its declaration in 
	//		function_pointers.h
}
