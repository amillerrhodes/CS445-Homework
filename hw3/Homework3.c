#include<stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int
main(void) {
    struct timeval tv;
    FILE* test_fptr;
    int test_fd;
    int i, t;
    int bufsizes[10] = {16, 32, 64, 128, 512, 1024, 2048, 4096, 8192, 16384};
    char* buf;

    printf("File Opening Benchmarks\n");
    /* fopen benchmark */
    gettimeofday(&tv, NULL); 
    t = tv.tv_sec * 1000000 + tv.tv_usec;
    test_fptr = fopen("testfile", "r+");
    gettimeofday(&tv, NULL); 
    printf("\tfopen \t %2d microseconds.\n", (tv.tv_sec * 1000000 + tv.tv_usec)-t);

    /* open benchmark */
    gettimeofday(&tv, NULL); 
    t = tv.tv_sec * 1000000 + tv.tv_usec;
    test_fd = open("testfile", O_RDWR);
    gettimeofday(&tv, NULL); 
    printf("\topen \t %2d microseconds.\n", (tv.tv_sec * 1000000 + tv.tv_usec)-t);

    int modes[3] = {_IOFBF, _IOLBF, _IONBF};


    printf("\nFile Read/Write Benchmarks\n");
    for (int i=0; i<10; i++) {
        buf = malloc(sizeof(char)*bufsizes[i]);
        for (int k=0; k<bufsizes[i]; k++) buf[k] = "0";
        printf("\nBuffer Size is %d\n", bufsizes[i]);

        printf("\t\t\tfwrite\tfread\twrite\tread\n", bufsizes[i]);
        for (int j=0; j<3; j++) {
            setvbuf(test_fptr, buf, modes[j], bufsizes[i]);
            if (j==0)
                printf("\tFully Buffered\t");
            else if (j==1)
                printf("\tLine Buffered\t");
            else if (j==2)
                printf("\tNot Buffered\t");

            /* fwrite benchmark */
            gettimeofday(&tv, NULL); 
            t = tv.tv_sec * 1000000 + tv.tv_usec;
            fwrite(buf, sizeof(char), 10, test_fptr);
            gettimeofday(&tv, NULL); 
            printf("%2dus\t", (tv.tv_sec * 1000000 + tv.tv_usec)-t);

            /* fread benchmark */
            gettimeofday(&tv, NULL); 
            t = tv.tv_sec * 1000000 + tv.tv_usec;
            fread(buf, sizeof(char), 10, test_fptr);
            gettimeofday(&tv, NULL); 
            printf("%2dus\t", (tv.tv_sec * 1000000 + tv.tv_usec)-t);

            /* write benchmark */
            gettimeofday(&tv, NULL); 
            t = tv.tv_sec * 1000000 + tv.tv_usec;
            write(test_fd, buf, sizeof(char)*10);
            gettimeofday(&tv, NULL); 
            printf("%2dus\t", (tv.tv_sec * 1000000 + tv.tv_usec)-t);

            /* read benchmark */
            gettimeofday(&tv, NULL); 
            t = tv.tv_sec * 1000000 + tv.tv_usec;
            read(test_fd, buf, sizeof(char)*10);
            gettimeofday(&tv, NULL); 
            printf("%2dus\n", (tv.tv_sec * 1000000 + tv.tv_usec)-t);
        }
        free(buf);
    }

    printf("\nFile Closing Benchmarks\n");
    /* close benchmark */
    gettimeofday(&tv, NULL); 
    t = tv.tv_sec * 1000000 + tv.tv_usec;
    close(test_fd);
    gettimeofday(&tv, NULL); 
    printf("\tclose \t %2d microseconds.\n", (tv.tv_sec * 1000000 + tv.tv_usec)-t);

    /* fclose benchmark */
    gettimeofday(&tv, NULL); 
    t = tv.tv_sec * 1000000 + tv.tv_usec;
    fclose(test_fptr);
    gettimeofday(&tv, NULL); 
    printf("\tfclose \t %2d microseconds.\n", (tv.tv_sec * 1000000 + tv.tv_usec)-t);

    return 0;
}
