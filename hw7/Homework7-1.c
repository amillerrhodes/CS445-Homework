#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#define MAXLINE 4096
#define DEF_PAGER   "/bin/more"     /* default pager program */
int
main(int argc, char *argv[])
{
    int    n;
    int    fd[2];
    pid_t  pid;
    char   *pager, *argv0;
    char   line[MAXLINE];
    FILE   *fp;
    if (argc != 2) {
        printf("usage: a.out <pathname>");
        exit(1);
    }
    if ((fp = fopen(argv[1], "r")) == NULL) {
        perror("fopen error"); exit(1);
    }
    if (pipe(fd) < 0) {
        perror("pipe error"); exit(1);
    }
    if ((pid = fork()) < 0) {
        perror("fork error"); exit(1);
    } else if (pid > 0) {                              /* parent */
        close(fd[0]);       /* close read end */
        /* parent copies argv[1] to pipe */
        while (fgets(line, MAXLINE, fp) != NULL) {
            n = strlen(line);
            if (write(fd[1], line, n) != n) {
                perror("write error to pipe"); exit(1);
            }
        }
        if (ferror(fp)) {
            perror("fgets error"); exit(1);
        }
        //close(fd[1]);   /* close write end of pipe for reader */
        if (waitpid(pid, NULL, 0) < 0) {
            perror("waitpid error"); exit(1);
        }
        exit(0);
    } else {                                        /* child */
        close(fd[1]);   /* close write end */
        if (fd[0] != STDIN_FILENO) {
            if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO) {
                perror("dup2 error to stdin"); exit(1);
            }
            close(fd[0]);   /* don't need this after dup2 */
        }
        /* get arguments for execl() */
        if ((pager = getenv("PAGER")) == NULL)
            pager = DEF_PAGER;
        if ((argv0 = strrchr(pager, '/')) != NULL)
            argv0++;        /* step past rightmost slash */
        else
            argv0 = pager;  /* no slash in pager */
        if (execl(pager, argv0, (char *)0) < 0) {
            perror("execl error"); exit(1);
        }
    }
    exit(0);
}
