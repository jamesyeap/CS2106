#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

#include "packer.h"

/* integer-code representing each color */
#define RED 1
#define GREEN 2 
#define BLUE 3

/* a struct representing a packing-area for each of the colours */
struct PACKING_AREA {    
    int *id_list; // contains the id of all the balls in the box
    int n_packing_area; // tracks how many balls are inside the packing-area
    sem_t packing_barrier_mutex; // stops balls from being packed until there are enough balls inside the packing-area
    int n_balls_packed; // tracks how many balls inside the packing-area have been packed
};

/* function declarations for my own helper functions */
void init_packing_area(struct PACKING_AREA *area, sem_t *access_mutex);
void init_waiting_area(sem_t *waiting_area_sem);
void destroy(struct PACKING_AREA *area, sem_t *access_mutex, sem_t *waiting_area_sem);
void get_other_ball_id(int **list_ptr, int id, int *other_ids);

// You can declare global variables here
struct PACKING_AREA red_packing_area;
sem_t red_access_mutex;
sem_t red_waiting_area_sem;

struct PACKING_AREA green_packing_area;
sem_t green_access_mutex;
sem_t green_waiting_area_sem;

struct PACKING_AREA blue_packing_area;
sem_t blue_access_mutex;
sem_t blue_waiting_area_sem;

int N; // number of balls required to be they can be packed into a box

void packer_init(int balls_per_pack) {
    // Write initialization code here (called once at the start of the program).
    N = balls_per_pack;

    init_packing_area(&red_packing_area, &red_access_mutex);
    init_waiting_area(&red_waiting_area_sem);

    init_packing_area(&green_packing_area, &green_access_mutex);
    init_waiting_area(&green_waiting_area_sem);

    init_packing_area(&blue_packing_area, &blue_access_mutex);
    init_waiting_area(&blue_waiting_area_sem);    
}

void packer_destroy(void) {
    // Write deinitialization code here (called once at the end of the program).    

    destroy(&red_packing_area, &red_access_mutex, &red_waiting_area_sem);
    destroy(&green_packing_area, &green_access_mutex, &green_waiting_area_sem);
    destroy(&blue_packing_area, &blue_access_mutex, &blue_waiting_area_sem);
}

void pack_ball(int colour, int id, int *other_ids) {
    // Write your code here.    

    if (colour == RED) {         
        sem_wait(&red_waiting_area_sem); 

        sem_wait(&red_access_mutex);

        red_packing_area.id_list[red_packing_area.n_packing_area] = id;
        red_packing_area.n_packing_area++;        

        if (red_packing_area.n_packing_area == N) {          
            sem_post(&(red_packing_area.packing_barrier_mutex));

            sem_post(&red_access_mutex);

            sem_wait(&(red_packing_area.packing_barrier_mutex));

            sem_wait(&red_access_mutex);

            get_other_ball_id(&(red_packing_area.id_list), id, other_ids);
            red_packing_area.n_balls_packed++;            
        } else {                        
            sem_post(&red_access_mutex);
            
            sem_wait(&(red_packing_area.packing_barrier_mutex));            

            sem_wait(&red_access_mutex);

            get_other_ball_id(&(red_packing_area.id_list), id, other_ids);
            red_packing_area.n_balls_packed++;
        }

        // start cleanup process for the next-batch if all the balls in the packing-area have been processed
        if (red_packing_area.n_balls_packed == N) {                    
            init_packing_area(&red_packing_area, &red_access_mutex);

            for (int i=0; i<N; i++) {
                sem_post(&red_waiting_area_sem);
            }
        } else {
            sem_post(&(red_packing_area.packing_barrier_mutex));
        }

        sem_post(&red_access_mutex);           
    } else if (colour == GREEN) {
        sem_wait(&green_waiting_area_sem); 

        sem_wait(&green_access_mutex);

        green_packing_area.id_list[green_packing_area.n_packing_area] = id;
        green_packing_area.n_packing_area++;        

        if (green_packing_area.n_packing_area == N) {          
            sem_post(&(green_packing_area.packing_barrier_mutex));

            sem_post(&green_access_mutex);

            sem_wait(&(green_packing_area.packing_barrier_mutex));

            sem_wait(&green_access_mutex);

            get_other_ball_id(&(green_packing_area.id_list), id, other_ids);
            green_packing_area.n_balls_packed++;            
        } else {                        
            sem_post(&green_access_mutex);
            
            sem_wait(&(green_packing_area.packing_barrier_mutex));            

            sem_wait(&green_access_mutex);

            get_other_ball_id(&(green_packing_area.id_list), id, other_ids);
            green_packing_area.n_balls_packed++;
        }

        // start cleanup process for the next-batch if all the balls in the packing-area have been processed
        if (green_packing_area.n_balls_packed == N) {                    
            init_packing_area(&green_packing_area, &green_access_mutex);

            for (int i=0; i<N; i++) {
                sem_post(&green_waiting_area_sem);
            }
        } else {
            sem_post(&(green_packing_area.packing_barrier_mutex));
        }

        sem_post(&green_access_mutex);          
    } else { // colour == BLUE
        sem_wait(&blue_waiting_area_sem); 

        sem_wait(&blue_access_mutex);

        blue_packing_area.id_list[blue_packing_area.n_packing_area] = id;
        blue_packing_area.n_packing_area++;        

        if (blue_packing_area.n_packing_area == N) {          
            sem_post(&(blue_packing_area.packing_barrier_mutex));

            sem_post(&blue_access_mutex);

            sem_wait(&(blue_packing_area.packing_barrier_mutex));

            sem_wait(&blue_access_mutex);

            get_other_ball_id(&(blue_packing_area.id_list), id, other_ids);
            blue_packing_area.n_balls_packed++;            
        } else {                        
            sem_post(&blue_access_mutex);
            
            sem_wait(&(blue_packing_area.packing_barrier_mutex));            

            sem_wait(&blue_access_mutex);

            get_other_ball_id(&(blue_packing_area.id_list), id, other_ids);
            blue_packing_area.n_balls_packed++;
        }

        // start cleanup process for the next-batch if all the balls in the packing-area have been processed
        if (blue_packing_area.n_balls_packed == N) {                    
            init_packing_area(&blue_packing_area, &blue_access_mutex);

            for (int i=0; i<N; i++) {
                sem_post(&blue_waiting_area_sem);
            }
        } else {
            sem_post(&(blue_packing_area.packing_barrier_mutex));
        }

        sem_post(&blue_access_mutex);           
    }            
}

/* my own helper functions */
void init_packing_area(struct PACKING_AREA *area, sem_t *access_mutex) {
    area->id_list = malloc(sizeof(int) * N);
    area->n_packing_area = 0;
    area->n_balls_packed = 0;
    int sem_1_init_success = sem_init(&(area->packing_barrier_mutex), 0, 0);
    int sem_2_init_success = sem_init(access_mutex, 0, 1);

    if (!area->id_list) {
        fprintf(stderr, "Out of memory!\n");
        abort();
    }

    if ((sem_1_init_success != 0) | (sem_2_init_success != 0)) {
        fprintf(stderr, "Failed to initialize semaphore!\n");
        abort();
    }
}

void init_waiting_area(sem_t *waiting_area_sem) {
    int sem_init_success = sem_init(waiting_area_sem, 0, N);

    if (sem_init_success != 0) {
        fprintf(stderr, "Failed to initialize semaphore!\n");
        abort();
    }
}

void destroy(struct PACKING_AREA *area, sem_t *access_mutex, sem_t *waiting_area_sem) {
    free(area->id_list);
    sem_destroy(&(area->packing_barrier_mutex));
    sem_destroy(access_mutex);
    sem_destroy(waiting_area_sem);
}

void get_other_ball_id(int **list_ptr, int id, int *other_ids) {    
    int count = 0;

    for (int i=0; i<N; i++) {
        if ((*list_ptr)[i] != id) {
            other_ids[count] = (*list_ptr)[i];
            count++;
        }
    }        
}