#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#define LOCKED_BYTES 10000

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define read_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
            lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

int cpid[10];

int
main(void) {
    int fd, i, child = 0, mybyte = 0, special_child = 0;
    pid_t pid;
    fd = open("templock", O_CREAT | O_RDWR, FILE_MODE);
    write(fd, "0123456789", 10);
    
    for (i=0; i<10; i++) {
        cpid[i] = fork();
        if (cpid[i] == 0) {
            child = 1;
            if (i > 5) {
                special_child = 1;
            }
            mybyte = i;
            break;
        }
    }
        
    if (child) {
        printf("CHILD %d (%ld): Read Locking file\n", getpid(), time(NULL));
        fflush(stdout);
        if(readw_lock(fd, mybyte, SEEK_SET, 1) != -1) {
            printf("CHILD %d (%ld): Read Locked file\n", getpid(), time(NULL));
        } else {
            perror("readw_lock");
        }
        sleep(10);
        if (special_child) {
            un_lock(fd, mybyte, SEEK_SET, 1);
            printf("CHILD %d (%ld): Unlocked Read Lock on file\n", getpid(), time(NULL));
            if(readw_lock(fd, mybyte, SEEK_SET, 1) != -1) {
                printf("CHILD %d (%ld): Read Locked file again\n", getpid(), time(NULL));
            } else {
                perror("readw_lock");
            }
        }
        sleep(5);
            
        printf("CHILD %d (%ld): Exiting\n", getpid(), time(NULL));
        //un_lock(fd, 0, SEEK_SET, 0);
    } else {
        sleep(1);
        printf("PARENT %d (%ld): Attempt to Write Lock file\n", getpid(), time(NULL));
        if(writew_lock(fd, 0, SEEK_SET, 0) == -1) {
            printf("PARENT %d (%ld): Unable to Write Lock file\n", getpid(), time(NULL));
        } else {
            printf("PARENT %d (%ld): Write Locked file\n", getpid(), time(NULL));
        }
    }

    return 0;
}


int
lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;
    lock.l_type = type;     /* F_RDLCK, F_WRLCK, F_UNLCK */
    lock.l_start = offset;  /* byte offset, relative to l_whence */
    lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END */
    lock.l_len = len;       /* #bytes (0 means to EOF) */
    return(fcntl(fd, cmd, &lock));
}
