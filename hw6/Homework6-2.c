#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int
main(int argc, char *argv[])
{
    int         fdin, fdout;
    void        *src, *dst;
    struct stat statbuf;
    if (argc != 3) {
        printf("usage: %s <fromfile> <tofile>", argv[0]);
        exit(0);
    }
    if ((fdin = open(argv[1], O_RDONLY)) < 0)
        perror("open error");
    if ((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC,
      FILE_MODE)) < 0)
        perror("open error");
    if (fstat(fdin, &statbuf) < 0)   /* need size of input file */
        perror("fstat error");
    /* set size of output file */
    if (lseek(fdout, statbuf.st_size - 1, SEEK_SET) == -1)
        perror("lseek error");
    if (write(fdout, "", 1) != 1)
        perror("write error");
    if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED,
      fdin, 0)) == MAP_FAILED)
        perror("mmap error for input");
    close(fdin);
    if ((dst = mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE,
      MAP_SHARED, fdout, 0)) == MAP_FAILED)
        perror("mmap error for output");
    memcpy(dst, src, statbuf.st_size); /* does the file copy */
    exit(0);
}
