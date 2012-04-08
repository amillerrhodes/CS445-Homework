#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define LINE_LENGTH 80

void sigusr1_handler(int sig);
void sigusr2_handler(int sig);
void sigalrm_handler(int sig);

FILE* ifp;
char line[LINE_LENGTH];
pid_t ppid = 0;
int file_empty = 0;

pid_t pid1 = 1;
pid_t pid2 = 1;

int main(int argc, char* argv[]) {
    ifp = fopen("input", "r");
    ppid = getpid();
    pid_t pid = getpid(); /* Parent ID */
    //pid_t pid1 = 1;       /* Child 1 ID */
    //pid_t pid2 = 1;       /* Child 2 ID */
    int status;
    int n = 0;

    char line[LINE_LENGTH];

    pid1 = fork();

    if (pid1 != 0) {
        pid2 = fork();
    }

    for (;;) {
        if (pid1 == 0) {
            signal(SIGALRM, sigalrm_handler);
            if (feof(ifp)) {
                exit(0);
            }
        } else if (pid2 == 0) {
            signal(SIGALRM, sigalrm_handler);
            if (feof(ifp)) {
                exit(0);
            }
        } else {

            /* Set up signal handlers */
            signal(SIGUSR1, sigusr1_handler); // child 1 is free
            signal(SIGUSR2, sigusr2_handler); // child 2 is free
            if(n == 0) { 
            /* going to wait one second at the start
             * to make sure that we don't send the kill before
             * our child's signal handler is set up
             */
                sleep(1);
                n = 1;
            }
            fflush(stdout);
            kill(pid2, SIGALRM);
            kill(pid1, SIGALRM);
            if (feof(ifp)) {
                kill(pid1, SIGTERM);
                kill(pid2, SIGTERM);
                exit(0);
            }
        }
    }

    return 0;
}


void
sigusr1_handler(int sig) {
    // child 1 is not busy any more
    kill(pid1, SIGALRM);
}

void
sigusr2_handler(int sig) {
    // child 2 is not busy any more
    kill(pid2, SIGALRM);
}

void
sigalrm_handler(int sig) {
    if(fgets(line, LINE_LENGTH, ifp) != NULL) {
        system(line);
    }
    if(pid1 == 0) {
        printf("\t => CHILD1 done\n");
        fflush(stdout);
        kill(ppid, SIGUSR1); 
    } else if (pid2 == 0) {
        printf("\t => CHILD2 done\n");
        fflush(stdout);
        kill(ppid, SIGUSR2);
    }
    fflush(stdout);
}

