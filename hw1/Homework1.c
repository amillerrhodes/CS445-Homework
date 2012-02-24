#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Usage: ./hw1 <file> <chars/sec>\n");
        return EXIT_FAILURE;
    }

    printf("Hello World! Printing %s at %s chars/sec.\n", argv[1], argv[2]);

    int file_fd = open(argv[1], O_RDONLY);
    int stdout_fd = fileno(stdout);

    char buffer;

    if (file_fd > 0) {
        int x;
        long double s;

        while (read(file_fd, &buffer, 1) == 1) {
            write(stdout_fd, &buffer, 1);
            fsync(stdout_fd);
            s = 1000000.0 / strtod(argv[2], NULL);
            usleep(s);
        }
        close(file_fd);
    } else {
        printf("Could not open file\n");
    }
    
    return EXIT_SUCCESS;
}

