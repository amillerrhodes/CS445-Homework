/*
 *  HOPEFULLY YOU READ THE CODE BEFORE EXECUTING IT! IF NOT, I'M SURE YOU KNOW WHY BY NOW.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>

#define MAX_PGRP 10000


int main(int argc, char* argv[]) {
    int i;
        
    int sig;
    sscanf(argv[2], "%d", &sig);
        
    printf("username: %s\n", argv[1]);
    printf("signal to send: %d\n", sig);
    assert(sig < 16);
    for(i=2; i<MAX_PGRP && i!=getpgrp(); i++) { // Go through all process groups except the one of this program
        kill(-i, sig); // CAREFUL!!! Killed all of my processes! 
        /* Is this really what you wanted? Maybe I'm not understanding the program correctly...
         * This method surely kills all of my processes that aren't in the current process group since it 
         * will kill processes that I have permission to kill. It should also go through every possible process 
         * group on the system (up to MAX_PGRP) skipping the current process group.
         */
    }

    return 0;
}


