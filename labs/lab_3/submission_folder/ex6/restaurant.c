#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/errno.h>

#include "restaurant.h"

void init_tables(int num_tables[5]);
void init_queue();
TICKET* get_ticket(group_state *state);
int try_serve_queue(int num_people, TABLE *curr_table, sem_t *curr_section_sem);

// You can declare global variables here
SECTION s[6]; // 1-indexed
pthread_mutex_t freeze;

QUEUE q;      

void restaurant_init(int num_tables[5]) {
    // Write initialization code here (called once at the start of the program).
    // It is guaranteed that num_tables is an array of length 5.
    // TODO

    init_tables(num_tables);
    init_queue();
}

void restaurant_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    // TODO

    for (int i=1; i<=5; i++) {
        sem_destroy(&s[i].available);

        for (int j=0; j<s[i].n_tables; j++) {
            pthread_mutex_destroy(&s[i].lst[j]->rw_t);
            free(s[i].lst[j]);
        }

        free(s[i].lst);        
    }

    
    pthread_mutex_destroy(&q.rw_q);

    for (int j=0; j<q.back; j++) {
        sem_destroy(&q.lst[j]->turn);
        free(q.lst[j]);
    }
}

int request_for_table(group_state *state, int num_people) {
    // Write your code here.
    // Return the id of the table you want this group to sit at.
    // TODO
    
    state->num_people = num_people;

    int found_table = 0;

    pthread_mutex_lock(&freeze);
    pthread_mutex_unlock(&freeze);

    for (int i=num_people; i<=5; i++) {             /* try to get an EMPTY table */
        if (sem_trywait(&s[i].available) == 0) {
            state->section_sem = &s[i].available;

            for (int j=0; j<s[i].n_tables; j++) {
                pthread_mutex_lock(&s[i].lst[j]->rw_t); ////////// CRITICAL SECTION //////////

                if (s[i].lst[j]->free_seats == s[i].lst[j]->total_seats) { // if this table is totally-free
                    state->t = s[i].lst[j];
                    s[i].lst[j]->free_seats -= num_people;

                    on_enqueue();
                    pthread_mutex_unlock(&s[i].lst[j]->rw_t); ///////////////////////////////////                       

                    break;
                } else {
                    pthread_mutex_unlock(&s[i].lst[j]->rw_t); ///////////////////////////////////                       
                }
            }

            found_table = 1;
            break;
        }
    }

    if (found_table == 0) {                     /* try to get an OCCUPIED table with the least number of extra seats */
        pthread_mutex_lock(&freeze); ////////// CRITICAL SECTION //////////

        TABLE *best_table = NULL;
        TABLE *curr_table;        

        for (int i=num_people; i<=5; i++) {

            for (int j=0; j<s[i].n_tables; j++) {
                curr_table = s[i].lst[j];                

                pthread_mutex_lock(&curr_table->rw_t); ////////// CRITICAL SECTION //////////

                if (curr_table->free_seats >= num_people) { // if the occupied-table has enough seats to seat this group
                    if (best_table == NULL) {
                        best_table = curr_table;                                                 
                    } else {
                        if (curr_table->free_seats == num_people) { // if the number of free-seats here is exactly the same as the number of people; this is the best possible fit, so stop searching further
                            best_table = curr_table;

                            pthread_mutex_unlock(&curr_table->rw_t); ///////////////////////////////////                            
                            break;
                        } else {
                            if (curr_table->free_seats < best_table->free_seats) {
                                best_table = curr_table;                
                            }
                        }
                    }

                    pthread_mutex_unlock(&curr_table->rw_t); ///////////////////////////////////                            
                } else {
                    pthread_mutex_unlock(&curr_table->rw_t); ///////////////////////////////////
                }               
            }
        }

        if (best_table != NULL) {
            state->t = best_table;
            state->section_sem = best_table->table_section_sem;
            
            best_table->free_seats -= num_people;

            on_enqueue();

            found_table = 1;
        }

        pthread_mutex_unlock(&freeze); ///////////////////////////////////
    }

    if (found_table == 0) {                     /* if either could not be found, join the queue */
        TICKET *t = get_ticket(state);   

        on_enqueue();     
        sem_wait(&t->turn);          
    }

    return state->t->id;
}

void leave_table(group_state *state) {
    // Write your code here.
    // TODO

    pthread_mutex_lock(&freeze);
    pthread_mutex_unlock(&freeze);

    try_serve_queue(state->num_people, state->t, state->t->table_section_sem);
}

/* HELPER FUNCTIONS */
void init_tables(int num_tables[5]) {
    pthread_mutex_init(&freeze, NULL);

    int curr_table_id=0;

    for (int i=0; i<5; i++) {
        s[i+1].n_tables = num_tables[i];
        sem_init(&s[i+1].available, 0, num_tables[i]);

        s[i+1].lst = malloc(sizeof(TABLE*) * num_tables[i]);

        for (int j=0; j<num_tables[i]; j++) {
            TABLE *t = malloc(sizeof(TABLE));
            t->id = curr_table_id;            
            pthread_mutex_init(&t->rw_t, NULL);
            t->total_seats = i+1;
            t->free_seats = i+1;
            t->table_section_sem = &s[i+1].available;

            s[i+1].lst[j] = t;

            curr_table_id++;
        }
    }
}

void init_queue() {
    q.back = 0;

    pthread_mutex_init(&q.rw_q, NULL);
}

TICKET* get_ticket(group_state *state) {
    pthread_mutex_lock(&q.rw_q); ////////// CRITICAL-SECTION //////////

    TICKET *t = malloc(sizeof(TICKET));
    t->id = q.back; 
    t->served = 0;
    t->g = state;
    sem_init(&t->turn, 0, 0);

    q.lst[t->id] = t;
    q.back++;

    pthread_mutex_unlock(&q.rw_q); ///////////////////////////////////

    return t;
}

int try_serve_queue(int num_people, TABLE *curr_table, sem_t *curr_section_sem) {
    int taken_over = 0;

    pthread_mutex_lock(&curr_table->rw_t); ////////// CRITICAL-SECTION //////////

    curr_table->free_seats += num_people;

    pthread_mutex_lock(&q.rw_q); ////////// CRITICAL-SECTION //////////

    for (int i=0; i<q.back; i++) {
        if (q.lst[i]->served == 0) { // if the group has not yet been served
            if (q.lst[i]->g->num_people <= curr_table->free_seats) { // and if the group can fit in this table
                TICKET *found_ticket = q.lst[i];
                group_state *suitable_group = found_ticket->g;                        

                suitable_group->t = curr_table;
                suitable_group->section_sem = curr_section_sem;
                found_ticket->served = 1;

                curr_table->free_seats -= suitable_group->num_people;               
                
                taken_over = 1;
                sem_post(&found_ticket->turn);

                break;
            }
        }
    }

    if (taken_over == 0) { // if no groups could be found to take over this table         
        if (curr_table->free_seats == curr_table->total_seats) { // check if this table is now empty
            sem_post(curr_section_sem);
        }
    }

    pthread_mutex_unlock(&q.rw_q); ///////////////////////////////////
    pthread_mutex_unlock(&curr_table->rw_t); ///////////////////////////////////

    return taken_over;
}





