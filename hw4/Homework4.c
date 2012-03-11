#include <stdio.h>
#include <stdlib.h>

#define LINE_LENGTH 80

int process_command(void);

int main(void) {
    pid_t pid = getpid(); /* Parent ID */
    pid_t pid1 = 1;       /* Child 1 ID */
    pid_t pid2 = 1;       /* Child 2 ID */
    int status;
    int n_children_running = 0;
    int command_executed = 0;
    pid_t exit_pid;

    char line[LINE_LENGTH];
    FILE* ifp = fopen("in.list", "r");


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

    char* args[2] = {"sleep", "30"};
    while (fgets(line, LINE_LENGTH, ifp) != NULL) {
        command_executed = 0;
        while (!command_executed) {
            if (pid1 == 0) {
                printf("In child 1 :: \n");
                fflush(stdout);
                //execvp(args[0], args);
                system(line);
                command_executed = 1;
                //exit(1);
            } else if (pid2 == 0) {
                args[1] = "20";
                printf("In child 2 :: \n");
                fflush(stdout);
                //execvp(args[0], args);
                system(line);
                command_executed = 1;
                //exit(1);
            } else {
                printf("In parent :: \n");
                if (n < 2) {
                    exit_pid = wait(&status);
                    printf("%d exited\n", exit_pid);
                    n++;
                } else {
                    exit(0);
                }
            }
        }
    }

    return 0;
}


