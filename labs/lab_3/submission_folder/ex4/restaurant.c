#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#include "restaurant.h"

void init_tables(int num_tables[5]);
void init_queues();
TICKET* get_ticket(group_state *state);
int try_serve_queue(int num_people, TABLE *curr_table, sem_t *curr_section_sem);

// You can declare global variables here
SECTION s[6]; // 1-indexed
QUEUE q[6]; // 1-indexed

void restaurant_init(int num_tables[5]) {
    // Write initialization code here (called once at the start of the program).
    // It is guaranteed that num_tables is an array of length 5.
    // TODO

    init_tables(num_tables);
    init_queues();
}

void restaurant_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).
    // TODO

    for (int i=1; i<=5; i++) {
        sem_destroy(&s[i].available);

        for (int j=0; j<s[i].n_tables; j++) {
            pthread_mutex_destroy(&s[i].lst[j]->a);
            free(s[i].lst[j]);
        }

        free(s[i].lst);        
    }

    for (int i=1; i<=5; i++) {
        pthread_mutex_destroy(&q[i].rw_q);

        for (int j=0; j<q[i].back; j++) {
            sem_destroy(&q[i].lst[j]->turn);
            free(q[i].lst[j]);
        }        
    }
}

int request_for_table(group_state *state, int num_people) {
    // Write your code here.
    // Return the id of the table you want this group to sit at.
    // TODO
    
    state->num_people = num_people;

    if (sem_trywait(&s[num_people].available) == 0) {
        state->section_sem = &s[num_people].available;
        
        for (int i=0; i<s[num_people].n_tables; i++) {
            if (pthread_mutex_trylock(&s[num_people].lst[i]->a) == 0) {                
                state->t = s[num_people].lst[i];

                on_enqueue();
                break;
            }            
        }
    } else {
        TICKET *t = get_ticket(state);   

        on_enqueue();     
        sem_wait(&t->turn);          
    }

    return state->t->id;
}

void leave_table(group_state *state) {
    // Write your code here.
    // TODO

    try_serve_queue(state->num_people, state->t, state->section_sem);
}

/* HELPER FUNCTIONS */
void init_tables(int num_tables[5]) {
    int curr_table_id=0;

    for (int i=0; i<5; i++) {
        s[i+1].n_tables = num_tables[i];
        sem_init(&s[i+1].available, 0, num_tables[i]);

        s[i+1].lst = malloc(sizeof(TABLE*) * num_tables[i]);

        for (int j=0; j<num_tables[i]; j++) {
            TABLE *t = malloc(sizeof(TABLE));
            t->id = curr_table_id;            
            pthread_mutex_init(&t->a, NULL);

            s[i+1].lst[j] = t;

            curr_table_id++;
        }
    }
}

void init_queues() {
    for (int i=1; i<=5; i++) {
        q[i].front = 0;
        q[i].back = 0;

        pthread_mutex_init(&q[i].rw_q, NULL);
    }
}

TICKET* get_ticket(group_state *state) {
    pthread_mutex_lock(&q[state->num_people].rw_q); ////////// CRITICAL-SECTION //////////

    TICKET *t = malloc(sizeof(TICKET));
    t->id = q[state->num_people].back; 
    t->g = state;
    sem_init(&t->turn, 0, 0);

    q[state->num_people].lst[t->id] = t;
    q[state->num_people].back++;

    pthread_mutex_unlock(&q[state->num_people].rw_q); ///////////////////////////////////

    return t;
}

int try_serve_queue(int num_people, TABLE *curr_table, sem_t *curr_section_sem) {
    int taken_over = 0;

    pthread_mutex_lock(&q[num_people].rw_q); ////////// CRITICAL-SECTION //////////

    if (q[num_people].front != q[num_people].back) { // if there are people in the queue
        TICKET *found_ticket = q[num_people].lst[q[num_people].front];
        group_state *suitable_group = found_ticket->g;        

        suitable_group->t = curr_table;
        suitable_group->section_sem = curr_section_sem;

        q[num_people].front++;
        taken_over = 1;

        sem_post(&found_ticket->turn);
    }

    pthread_mutex_unlock(&q[num_people].rw_q); ///////////////////////////////////

    if (taken_over == 0) { // if no groups could be found to take over this table
        pthread_mutex_unlock(&curr_table->a); // free this table
        sem_post(curr_section_sem);             
    }

    return taken_over;
}