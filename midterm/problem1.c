/*
 * NOTES:
 * -- How do I share a file between processes? 
 *
 *
 *
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define LINE_LENGTH 80

void sigusr1_handler(int sig);
void sigusr2_handler(int sig);
void sigalrm_handler(int sig);

static busy = 0;

int main(void) {
    pid_t pid0 = getpid(); /* Parent ID */
    pid_t pid1 = 1;       /* Child 1 ID */
    pid_t pid2 = 1;       /* Child 2 ID */
    pid_t pid3 = 1;       /* Child 2 ID */
    int status;
    int n_children_running = 0;
    int command_executed = 0;
    pid_t exit_pid;

    char line[LINE_LENGTH];
    FILE* ifp = fopen("input", "r");


    pid1 = fork();
    if (pid1 == 0) 
        pid2 = fork();
    if (pid2 == 0) 
        pid3 = fork();

    for (;;) {
        if (pid1 == 0) { 
            signal(SIGUSR1, sigusr1_handler);
            if(busy) {
                printf("Child 1 busy\n");
                fflush(stdout);
                kill(pid2, SIGUSR1);
            } else {
                busy = 1;
                if(fgets(line, LINE_LENGTH, ifp) != NULL) {
                    system(line);
                    busy = 0;
                }
            }
        } else if (pid2 == 0) {
            signal(SIGUSR1, sigusr1_handler);
            if(busy) {
                printf("Child 2 busy\n");
                fflush(stdout);
                kill(pid3, SIGUSR1);
            } else {
                busy = 1;
                if(fgets(line, LINE_LENGTH, ifp) != NULL) {
                    system(line);
                    busy = 0;
                }
            }
        } else if (pid3 == 0) {
            signal(SIGUSR1, sigusr1_handler);
            if(busy) {
                printf("Child 3 busy\n");
                fflush(stdout);
                kill(pid0, SIGUSR1);
            } else {
                busy = 1;
                if(fgets(line, LINE_LENGTH, ifp) != NULL) {
                    system(line);
                    busy = 0;
                }
            }
        } else {
            signal(SIGUSR1, sigusr1_handler);
            if(busy) {
                printf("Parent busy\n");
                fflush(stdout);
                kill(pid1, SIGUSR1);
            } else {
                busy = 1;
                if(fgets(line, LINE_LENGTH, ifp) != NULL) {
                    system(line);
                    busy = 0;
                }
            }

            sleep(1);
            kill(pid1, SIGUSR1);
        }
    }

    return 0;
}


void
sigusr1_handler(int sig) {
}

void
sigusr2_handler(int sig) {
}

void
sigalrm_handler(int sig) {
}

