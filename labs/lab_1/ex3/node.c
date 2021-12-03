/*************************************
* Lab 1 Exercise 3
* Name: Yeap Yi Sheng James
* Student No: A0218234L
* Lab Group: B09
*************************************/

#include "node.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Copy in your implementation of the functions from ex2.
// There is one extra function called map which you have to fill up too.
// Feel free to add any new functions as you deem fit.

// Inserts a new node with data value at index (counting from head
// starting at 0).
// Note: index is guaranteed to be valid.
void insert_node_at(list *lst, int index, int data) {
	node *new_node_ptr = (node *)malloc(sizeof(node));
	new_node_ptr->data = data; 

	if (index == 0) {
		// if you want to insert at the start of the list
		if (lst->head == NULL) 
		{ // if list is currently empty
			lst->head = new_node_ptr;
			new_node_ptr->next = new_node_ptr; 
			return;
		}

		node *curr_node = lst->head; // <-- set to the first node
		// get the last node
		while (curr_node->next != lst->head)
		{ 
			curr_node = curr_node->next;
		}
		curr_node->next = new_node_ptr; // <-- at this point, curr_node is the last node
		new_node_ptr->next = lst->head;
		lst->head = new_node_ptr; 
	} else {		
		node *curr_node_ptr = lst->head; 

		for (int i=0; i<index-1; i++) 
		{
			curr_node_ptr = curr_node_ptr->next; 
		}

		new_node_ptr->next = curr_node_ptr->next;
		curr_node_ptr->next = new_node_ptr;
	}
}

// Deletes node at index (counting from head starting from 0).
// Note: index is guarenteed to be valid.
void delete_node_at(list *lst, int index) {
	if (lst->head == NULL)
	{
		// do nothing
		return;
	}

	if (lst->head->next == lst->head) 
	{ // IF list only has 1 element
		free(lst->head->next);
		lst->head = NULL;	
		return;
	}

	if (index == 0)
	{ // delete node at the start of the list			
		// IF list has >1 element
			node *node_to_delete = lst->head;

			node *last_node = lst->head;
			while (last_node->next != lst->head)
			{
				last_node = last_node->next;
			}
			last_node->next = node_to_delete->next; 
			lst->head = node_to_delete->next;
			free(node_to_delete);	
	} else {
		// access the node that is before the node to be deleted
		node *curr_node = lst->head;
		for (int i=0; i<index-1; i++)
		{
			curr_node = curr_node->next;
		}

		node *node_to_delete = curr_node->next; 
		curr_node->next = node_to_delete->next;
		free(node_to_delete);
	}
}

// Rotates list by the given offset.
// Note: offset is guarenteed to be non-negative.
void rotate_list(list *lst, int offset) {
	// note: shifts elements on the LEFT of the offset index to its RIGHT-side
	//			shifting process starts from the FIRST element

	// just need to change the list's head to the second node
	for (int i=0; i<offset; i++) 
	{
		lst->head = lst->head->next;
	}
}

// Reverses the list, with the original "tail" node
// becoming the new head node.
void reverse_list(list *lst) {
	if (lst->head == NULL)
	{
		// do nothing, since an empty list cannot be reversed
		return ;
	}

	node *prev_node_ptr = lst->head;
	node *curr_node_ptr = prev_node_ptr->next; 
	node *next_node_ptr = curr_node_ptr->next;

	while (next_node_ptr->next != lst->head) 
	{
		curr_node_ptr->next = prev_node_ptr;

		prev_node_ptr = curr_node_ptr;
		curr_node_ptr = next_node_ptr;
		next_node_ptr = next_node_ptr->next;
	}

	curr_node_ptr->next = prev_node_ptr;
	next_node_ptr->next = curr_node_ptr;

	lst->head->next = next_node_ptr;
	lst->head = next_node_ptr;
}

// Resets list to an empty state (no nodes) and frees
// any allocated memory in the process
void reset_list(list *lst) {
	if (lst->head == NULL)
	{
		// do nothing, since list is already empty my dudeeeeeee.
	} else {
		while (lst->head != NULL)
		{
			delete_node_at(lst, 0);
		}
	}
}

// Traverses list and applies func on data values of
// all elements in the list.
void map(list *lst, int (*func)(int)) {
	if (lst->head == NULL)
	{
		// do nothing, since no operations can be performed on an empty list
		return ;
	}
	node *curr_node = lst->head; 
	while (curr_node->next != lst->head)
	{
		// perform operation and modify the node data
		curr_node->data = (*func)(curr_node->data);
		// move on to next node
		curr_node = curr_node->next;
	}
	// perform operation and modify the node data
	curr_node->data = (*func)(curr_node->data);
}

// Traverses list and returns the sum of the data values
// of every node in the list.
long sum_list(list *lst) {
	long int result = 0;
	if (lst->head == NULL)
	{
		printf("%ld\n", result);
		return result;
	} else 
	{
		long int test_result = lst->head->data;
		test_result += lst->head->data;

		node *curr_node = lst->head; 
		while (curr_node->next != lst->head)
		{
			// add data in each node to the sum
			result += (curr_node->data); 
			// move on to next node
			curr_node = curr_node->next;
		}
		result += curr_node->data;

		printf("%ld\n", result);
		return result;
	}
}

