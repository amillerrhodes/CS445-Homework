#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>

#define LINE_LENGTH 80

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int fd;

char* readline(int fd, char* buffer) {
    int i = 0;
    for (; read(fd, buffer+i, 1) == 1 && buffer[i] != '\n'; i++) {}
    if (i == 0)
        return NULL;
    buffer[i] = '\0';
    return buffer;
}

void*
thr_fn(void *arg)
{
	pid_t		pid;
	pthread_t	tid;
    char* buffer = malloc(sizeof(char)* LINE_LENGTH);

	pid = getpid();
	tid = pthread_self();
    pthread_mutex_lock(&mutex);
    printf("Thread %d Locking mutex\n", tid, buffer);
    fflush(stdout);
    //readline...
    if ((buffer = readline(fd, buffer)) != NULL) {
        buffer[strlen(buffer)] = '\0';
    }
    printf("Thread %d read %s from file\nUnlocking mutex\n", tid, buffer);
    fflush(stdout);
    pthread_mutex_unlock(&mutex);
    printf("Thread %d starting to execute %s\n", tid, buffer);
    fflush(stdout);
    system(buffer);
    printf("Thread %d finished\n", tid);
    fflush(stdout);
	return((void *)0);
}


int
main(void)
{
	int	err, i = 0, j;
    pthread_t* threadids = malloc(sizeof(pthread_t)*100);
    char* buffer = malloc(sizeof(char)*LINE_LENGTH);
    fd = open("input", O_RDONLY);


    printf("Main Process counting number of lines\n");
    fflush(stdout);
    while((buffer = readline(fd, buffer)) != NULL) {
        buffer[strlen(buffer)] = '\0';
        i++;
        usleep(10000);
    }
    lseek(fd, 0, SEEK_SET);

    free(buffer);

    printf("Main Process spawning threads\n");
    for(j=0; j<i; j++) {
	    err = pthread_create(threadids+j, NULL, thr_fn, NULL);
        if (err != 0) {
            perror("pthread_create error");
        }
    }

    printf("Main Process waiting for threads to finish\n");
    while (--i > 0) {
        pthread_join(*(threadids+i), NULL);
        i--;
    }

    free(threadids);

    sleep(1);
	exit(0);
}
