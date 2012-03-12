#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define LINE_LENGTH 80

int process_command(void);
void sigusr1_handler(int sig);
void sigusr2_handler(int sig);
void sigalrm_handler(int sig);

int main(void) {
    pid_t pid = getpid(); /* Parent ID */
    pid_t pid1 = 1;       /* Child 1 ID */
    pid_t pid2 = 1;       /* Child 2 ID */
    int status;
    int n_children_running = 0;
    int command_executed = 0;
    pid_t exit_pid;

    char line[LINE_LENGTH];
    FILE* ifp = fopen("input", "r");


    int n = 0;
    /* Start child 1 */
    pid1 = fork();
    n_children_running++;

    /* Start child 2
     * Need the if clause to make sure that the first child doesn't fork its
     *  own child
     */
    if (pid1 != 0) {
        pid2 = fork();
        n_children_running++;
    }

    int i = 0;
    //while (fgets(line, LINE_LENGTH, stdin) != NULL) {
    //while (!feof(ifp)) { // Probably should just do infinite loop and catch sigint to clean up.
    for (;;i++) {
        command_executed = 0;
        fgets(line, LINE_LENGTH, ifp);
        printf("Executing line %d\n", i);
        while (!command_executed) {
            if (pid1 == 0) {
                signal(SIGALRM, sigalrm_handler);
                //printf("In child 1\n");
                fflush(stdout);
                system(line);
                command_executed = 1;
                //exit(1);
            } else if (pid2 == 0) {
                signal(SIGALRM, sigalrm_handler);
                //printf("In child 2\n");
                fflush(stdout);
                system(line);
                command_executed = 1;
                //exit(1);
            } else {
                printf("In parent :: \n");

                /* Set up signal handlers */
                signal(SIGUSR1, sigusr1_handler); // child 1 is free
                signal(SIGUSR2, sigusr2_handler); // child 2 is free
                if (feof(ifp)) {
                    kill(pid1, SIGTERM);
                    kill(pid2, SIGTERM);
                }

                if (n < 2) {
                    exit_pid = wait(&status);
                    printf("%d exited\n", exit_pid);
                    n++;
                } else {
                    kill(pid1, SIGTERM);
                    kill(pid2, SIGTERM);
                    exit(0);
                }
            }
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

