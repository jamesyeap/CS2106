/**
 * CS2106 AY21/22 Semester 1 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */

/* TEST COMMANDS

------------- Ex. 1 -------------
info
/bin/echo hello
/bin/notaprogram
/bin/sleep 10 &
./programs/result 7
./programs/result 256
./programs/showCmdArg 5 23 1 &
quit

------------- Ex. 2 -------------
/bin/sleep 10 &
wait <pid>

/bin/sleep 30 &
terminate <pid>

./programs/lazy &
terminate <pid>
info

<------------ Note: this line will have problem if WIFEXITED() is not used together with WEXITSTATUS()
/bin/sleep 3 && /bin/echo Hi && /bin/echo Bye 

/bin/notaprogram && /bin/echo Hello

<----------------- Note: sleep() with invalid arg somehow doesn't throw an error on MacOS; should try on Ubuntu later
/bin/sleep notanumber && /bin/echo Hello 

/bin/echo Hi && /bin/notaprogram && /bin/echo Bye

/bin/echo A && ./programs/result 7 && /bin/echo B

/bin/echo A && ./programs/result 256 && /bin/echo B

./programs/groot &

quit

------------- Ex. 3 -------------
/bin/cat ./programs/result > ./a.txt
/bin/cat ./a.txt

/bin/sort < ./a.txt > ./b.txt &
/bin/cat ./b.txt

/bin/sort < ./doesnotexist.txt

/bin/printf world\nhello\n > ./a.txt && /bin/sort < ./a.txt > ./b.txt && /bin/cat ./b.txt 

/bin/echo hello && /bin/sort < ./doesnotexist.txt

/bin/sleep notanumber 2> ./a.txt && /bin/echo Hello
/bin/cat ./a.txt

quit
*/

/*  PSEUDO-CODE for PARSER (and possibly some actual code because can't be avoided)

SPLIT the array into multiple arrays by '&&'; make sure each array is NULL-terminated

FOR EACH array
    IF array has '&' inside
        MODIFY the array to have 1 less element; remove the '&' element
    
    IF array has '<' inside
        OPEN the file right after the '<' position in 'READ-ONLY' mode
            IF file doesn't exist
                EXIT
        
        MODIFY the array to have 2 less elements; 
            remove '<' element and the 'filename' element right after

    IF array has '>' inside
        OPEN the file right after the '>' position in 'WRITE' mode
            IF file doesn't exist
                CREATE the file with 'READ-WRITE' for USER and 'READ-ONLY' for OTHERS
        
        MODIFY the array to have 2 less elements; 
            remove '>' element and the 'filename' element right after

    IF array has '2>' inside
        OPEN the file right after the '2>' position in 'WRITE' mode
            IF file doesn't exist
                CREATE the file with 'READ-WRITE' for USER and 'READ-ONLY' for OTHERS
        
        MODIFY the array to have 2 less elements; 
            remove '2>' element and the 'filename' element right after

*/

#include "myshell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

/* FUNCTION PROTOTYPES */
int command_is(char *command, char **tokens);
void run_program(size_t num_tokens, char **tokens);
int execute_until_double_ampersand(size_t num_tokens, char **tokens, size_t start_index);
int check_existence(char *abs_path);
int start(size_t num_tokens, char **tokens);
int you_want_concurrent(size_t num_tokens, char **tokens);
int you_redirect_stdin(size_t num_tokens, char **tokens, int *fd);
int you_redirect_stdout(size_t num_tokens, char **tokens, int *fd);
int you_redirect_stderr(size_t num_tokens, char **tokens, int *fd);
void add_process_record(pid_t process_id);
void update_process_record(pid_t process_id, int updated_process_status, int exit_status);
void my_info();
void my_quit();
void my_wait(pid_t process_id);
void my_terminate(pid_t process_id);

/* STRUCT declaration */
struct PROCESS_CONTROL_BLOCK {
    pid_t pid;
    int process_status; // 0 for "exited", 1 for "running", 2 for "terminating"
    int exit_status;
};

/* My make-believe Process Table */
struct PROCESS_CONTROL_BLOCK process_table[50];
int processes_started = 0;

void my_init(void) {
    // Initialize what you need here
}

void my_process_command(size_t num_tokens, char **tokens) {
    // Your code here, refer to the lab document for a description of the arguments

    if (command_is("info", tokens) == 1) {
        my_info();
    } else if (command_is("wait", tokens) == 1) {
        my_wait(atoi(tokens[1]));
    } else if (command_is("terminate", tokens) == 1) {
        my_terminate(atoi(tokens[1]));
    } else {
        run_program(num_tokens, tokens);
    }
}

void my_quit(void) {
    // Clean up function, called after "quit" is entered as a user command

    for (int i=0; i<processes_started; i++) {
        if (process_table[i].process_status == 0) {
            //  if process has exited; do nothing

        } else if (process_table[i].process_status == 1) {
            //  if process is running; send SIGTERM to it and wait for it to exit

            kill(process_table[i].pid, SIGTERM);
            waitpid(process_table[i].pid, NULL, 0);
        } else {
            //  if process is terminating; wait for it to exit
            
            waitpid(process_table[i].pid, NULL, 0);
        }
    }

    printf("Goodbye!\n");
}

int command_is(char *command, char **tokens) {
    return strcmp(tokens[0], command) == 0;
}

void run_program(size_t num_tokens, char **tokens) {
    int curr_index = 0;

    while (curr_index < (int)num_tokens) {
        if (curr_index == -1) {
            break;
        }

        curr_index = execute_until_double_ampersand(num_tokens, tokens, curr_index);
    }    
}

int execute_until_double_ampersand(size_t num_tokens, char **tokens, size_t start_index) {
    char *buffer[100];

    for (size_t i=start_index; i<num_tokens-1; i++) {
        if (strcmp(tokens[i], "&&") == 0) {
            buffer[i - start_index] = NULL;

            //  STOP if the program does not exist
            if (check_existence(buffer[0]) == -1) {
                printf("%s not found\n", buffer[0]);

                return -1;
            }

            size_t new_num_tokens = i - start_index + 1;
            
            int success = start(new_num_tokens, buffer);

            //  STOP everything if a program fails
            if (success != 0) {
                printf("%s failed\n", buffer[0]);

                return -1;
            }

            return i + 1;
        }

        buffer[i - start_index] = tokens[i];
    }

    buffer[num_tokens - start_index - 1] = NULL;

    //  STOP if the program does not exist
    if (check_existence(buffer[0]) == -1) {
        printf("%s not found\n", buffer[0]);

        return -1;
    }

    size_t new_num_tokens = num_tokens - start_index;

    start(new_num_tokens, buffer);

    return num_tokens;
}

int check_existence(char *abs_path) {
    return access(abs_path, F_OK);
}

int start(size_t num_tokens, char **tokens) {
    int fd_0 = -1, fd_1 = -1, fd_2 = -1;

    int redirect_stdin = you_redirect_stdin(num_tokens, tokens, &fd_0);    
    int redirect_stdout = you_redirect_stdout(num_tokens, tokens, &fd_1);
    int redirect_stderr = you_redirect_stderr(num_tokens, tokens, &fd_2);

    if (redirect_stdin == -1) {
        /*  STOP if the file used for stdin redirection (<) cannot be accessed */
        return -1;
    }

    /*  CHECK if program should be run CONCURRENTLY */
    if (you_want_concurrent(num_tokens, tokens) == 1) {
        /* CONCURRENT case */

        pid_t child_pid = fork();

        if (child_pid == 0) {
            if (redirect_stdout == 1) {
                dup2(fd_1, STDOUT_FILENO);
            }

            if (redirect_stderr == 1) {
                dup2(fd_2, STDERR_FILENO);                
            }

            if (redirect_stdin == 1) {                
                dup2(fd_0, STDIN_FILENO);                

                execl(tokens[0], tokens[0], NULL);
                _exit(EXIT_FAILURE);
            }

            execv(tokens[0], tokens);
            _exit(EXIT_FAILURE); 
        } else {
            printf("Child[%d] in background\n", child_pid);
            add_process_record(child_pid);

            return 0;
        }
    } else {
        /* NON-CONCURRENT case */

        pid_t child_pid = fork();

        if (child_pid == 0) {
            if (redirect_stdout == 1) {                
                dup2(fd_1, STDOUT_FILENO);                
            }

            if (redirect_stderr == 1) {                
                dup2(fd_2, STDERR_FILENO);                
            }

            if (redirect_stdin == 1) {
                dup2(fd_0, STDIN_FILENO);                

                execl(tokens[0], tokens[0], NULL);
                _exit(EXIT_FAILURE);
            }

            execv(tokens[0], tokens);
            _exit(EXIT_FAILURE);
        } else {
            add_process_record(child_pid);

            int exit_status;
            int exited_child_pid = wait(&exit_status);

            update_process_record(exited_child_pid, 0, WEXITSTATUS(exit_status));

            //  STOP everything if a program fails
            if (WIFEXITED(exit_status) && WEXITSTATUS(exit_status) != 0) {
                return WEXITSTATUS(exit_status);
            } 

            return 0;
        }
    }
}

void add_process_record(pid_t process_id) {
    process_table[processes_started].pid = process_id;
    process_table[processes_started].process_status = 1; 

    processes_started++;
}

void update_process_record(pid_t process_id, int updated_process_status, int exit_status) {
    for (int i=0; i<processes_started; i++) {
        if (process_table[i].pid == process_id) {
            process_table[i].process_status = updated_process_status;

            if (updated_process_status == 0) {  //  if process has exited; update its exit status in the record
                process_table[i].exit_status = exit_status;
            }

            break;
        }
    }
}

int you_want_concurrent(size_t num_tokens, char **tokens) {
    
    for (size_t i=0; i<num_tokens-1; i++) {
        if (tokens[i] != NULL && strcmp(tokens[i], "&") == 0) {
            /* strip off the & */
            tokens[i] = NULL;

            return 1; 
        }
    }

    return 0;
}

int you_redirect_stdin(size_t num_tokens, char **tokens, int *fd) {
    for (size_t i=0; i<num_tokens-1; i++) {
        if (tokens[i] != NULL && strcmp(tokens[i], "<") == 0) {
            if (access(tokens[i+1], R_OK) == -1) {
                printf("%s does not exist\n", tokens[i+1]);

                return -1;
            } else {
                //  open the dang file my dude (in "R-only" mode)
                *fd = open(tokens[i+1], O_RDONLY);

                //  strip off the redirection symbol and filename
                tokens[i] = NULL;
                tokens[i+1] = NULL;

                return 1;
            }
        }
    }

    return 0;
}

int you_redirect_stdout(size_t num_tokens, char **tokens, int *fd) {
    for (size_t i=0; i<num_tokens-1; i++) {
        if (tokens[i] != NULL && strcmp(tokens[i], ">") == 0) {
            //  open the file; 
            //      if doesn't exist -> create one with "RW" for user and "R-only" for others
            //      if it exists -> overwrite its contents (using O_TRUNC) 
            *fd = open(tokens[i+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH);

            //  strip off the redirection symbol and filename
            tokens[i] = NULL;
            tokens[i+1] = NULL;

            return 1;
        }
    }

    return 0;
}

int you_redirect_stderr(size_t num_tokens, char **tokens, int *fd) {
    for (size_t i=0; i<num_tokens-1; i++) {
        if (tokens[i] != NULL && strcmp(tokens[i], "2>") == 0) {
            //  open the file; 
            //      if doesn't exist -> create one with "RW" for user and "R-only" for others
            //      if it exists -> overwrite its contents (using O_TRUNC)) 
            *fd = open(tokens[i+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH);

            //  strip off the redirection symbol and filename
            tokens[i] = NULL;
            tokens[i+1] = NULL;

            return 1;
        }
    }

    return 0;
}

//  Prints all processes started (including those that have exited)
void my_info() {
    for (int i=0; i<processes_started; i++) {
        struct PROCESS_CONTROL_BLOCK curr_block = process_table[i];

        if (curr_block.process_status == 2) {
            /* IF a SIGTERM signal was previously sent to the process */

            int curr_status;
            int any_changes = waitpid(process_table[i].pid, &curr_status, WNOHANG);

            if (any_changes == 0) {
                //  means process is still terminating

                printf("[%d] Terminating\n", curr_block.pid);
            } else if (any_changes == curr_block.pid) {
                //  means process experienced a status change -> means process has recently exited

                curr_block.process_status = 0; // update the process's status
                curr_block.exit_status = WEXITSTATUS(curr_status); // update the process's exit code

                printf("[%d] Exited %d\n", curr_block.pid, curr_block.exit_status);
            } else {
                //  means process has exited

                printf("[%d] Exited %d\n", curr_block.pid, curr_block.exit_status);
            }
        } else if (curr_block.process_status == 1) {
            /* IF the process was set to run concurrently in the background */

            int curr_status;
            int any_changes = waitpid(process_table[i].pid, &curr_status, WNOHANG);

            if (any_changes == 0) {
                //  means process is still running

                printf("[%d] Running\n", curr_block.pid);
            } else if (any_changes == curr_block.pid) {
                //  means process experienced a status change -> means process has recently exited

                curr_block.process_status = 0; // update the process's status
                curr_block.exit_status = WEXITSTATUS(curr_status); // update the process's exit code

                printf("[%d] Exited %d\n", curr_block.pid, curr_block.exit_status);
            } else {
                //  means process has exited
                
                printf("[%d] Exited %d\n", curr_block.pid, curr_block.exit_status);
            }
        } else {
            printf("[%d] Exited %d\n", curr_block.pid, curr_block.exit_status);
        }
    }
}

void my_wait(pid_t process_id) {
    for (int i=0; i<processes_started; i++) {
        if (process_table[i].pid == process_id) {
            if (process_table[i].process_status == 1) { //  if process is still running
                int exit_status;
                waitpid(process_id, &exit_status, 0);
                update_process_record(process_id, 0, WEXITSTATUS(exit_status));
            }

            break;
        }
    } 
}

void my_terminate(pid_t process_id) {
    for (int i=0; i<processes_started; i++) {
        if (process_table[i].pid == process_id) {
            if (process_table[i].process_status == 1) { //  if process is still running
                kill(process_id, SIGTERM);
                update_process_record(process_id, 2, -1); // arbitrary value of -1 for exit status cuz idk what to put here
            }

            break;
        }
    } 
}
