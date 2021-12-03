/*************************************
* Lab 1 Exercise 3
* Name: Yeap Yi Sheng James
* Student No: A0218234L
* Lab Group: B09
*************************************/

#include <stdio.h>
#include <stdlib.h>

#include "function_pointers.h"
#include "node.h"

// The runner is empty now! Modify it to fulfill the requirements of the
// exercise. You can use ex2.c as a template

// DO NOT initialize the func_list array in this file. All initialization
// logic for func_list should go into function_pointers.c.

// Macros
#define SUM_LIST 0
#define INSERT_AT 1
#define DELETE_AT 2
#define ROTATE_LIST 3
#define REVERSE_LIST 4
#define RESET_LIST 5
#define MAP 6

void run_instruction(list *lst, FILE *fp, int instr);

int main(int argc, char **argv) {
    // CHECK that the filename is given
    if (argc != 2) {
        fprintf(stderr, "Error: expecting 1 argument, %d found\n", argc - 1);
        exit(1);
    }

    // Update the array of function pointers
    // DO NOT REMOVE THIS CALL
    // (You may leave the function empty if you do not need it)
    update_functions();

    // Rest of code logic here

    /* OPEN the file */
    FILE *fp; 
    fp = fopen(argv[1], "r"); // <-- open filename in read-only mode
    // IF the filename is invalid
    if (fp == NULL)
    {
        printf("Oops the file \"%s\" cannot be found!\n Exiting program now...\n", argv[1]);
        return 1; // <-- returns non-zero exit code
    }

    /* MODIFIED FROM ex2 */
    // mainly changed scanf to fscanf
    
    list *lst = (list *)malloc(sizeof(list));
    lst->head = NULL;

    int instr;
    while (fscanf(fp, "%d", &instr) == 1) {
        run_instruction(lst, fp, instr);
    }

    reset_list(lst);
    free(lst);
    fclose(fp);
}

void run_instruction(list *lst, FILE *fp, int instr) {
    int index, data, offset, func_id;
    switch (instr) {
        case SUM_LIST:
            sum_list(lst);
            break;
        case INSERT_AT:
            // scanf("%d %d", &index, &data);
            fscanf(fp, "%d %d", &index, &data);
            insert_node_at(lst, index, data);
            break;
        case DELETE_AT:
            // scanf("%d", &index);
            fscanf(fp, "%d", &index);
            delete_node_at(lst, index);
            break;
        case ROTATE_LIST:
            // scanf("%d", &offset);
            fscanf(fp, "%d", &offset);
            rotate_list(lst, offset);
            break;
        case REVERSE_LIST:
            reverse_list(lst);
            break;
        case RESET_LIST:
            reset_list(lst);
            break;
        case MAP:
            fscanf(fp, "%d", &func_id);
            map(lst, func_list[func_id]);
            break;
    }
}
