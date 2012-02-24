#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<signal.h>

#define BUFFSIZE 4096

static void sigusr1_handler(int sig);
static void sigusr2_handler(int sig);
static void sigalrm_handler(int sig);

int
main(void) {
    int n; // length of filename to be read in
    int status;
    pid_t pid;

    char buffer[BUFFSIZE];

    struct stat buf;
    
    while ((n = read(STDIN_FILENO, buffer, BUFFSIZE)) > 0) {
        buffer[n-1] = 0; // Get rid of the newline
        
        pid = fork();

        if (pid == 0) {
            if (stat(buffer, &buf) == -1) {
                perror("stat error");
                exit(-1);
            }

            if (buf.st_size < 256) {
                if ((buf.st_size) % 2 == 0) {
                    kill(getppid(), SIGUSR2);
                } else {
                    kill(getppid(), SIGUSR1);
                }
                exit(buf.st_size);
            } else {
                kill(getppid(), SIGALRM);
                exit(0);
            }

        } else {
            signal(SIGUSR1, sigusr1_handler);
            signal(SIGUSR2, sigusr2_handler);
            signal(SIGALRM, sigalrm_handler);
            if (wait(&status) == -1)
                perror("wait error");
            if (WIFEXITED(status))
                printf("File Size: %d\n", WEXITSTATUS(status));
        }
    }
    return EXIT_SUCCESS;
}

void
sigusr1_handler(int sig) {
    printf("File size is odd.\n");
}

void
sigusr2_handler(int sig) {
    printf("File size is even.\n");
}

void
sigalrm_handler(int sig) {
    printf("File size exceeded 255 bytes. \n");
}

