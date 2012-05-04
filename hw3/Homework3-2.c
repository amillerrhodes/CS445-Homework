#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>

int
main (void) {
    int i; 


    /* Get Username */
    struct passwd* userinfo;
    gid_t* groups = malloc(32*sizeof(gid_t));
    struct group* groupinfo;
    int n, nr;
    while ((userinfo = getpwent()) != NULL) {
        printf("\n%s (%ld)\n", userinfo->pw_name, userinfo->pw_uid);

        /* Get Groups */
        //gid_t* groups = malloc(32*sizeof(gid_t));
        n = 32;
        nr = getgrouplist(userinfo->pw_name, userinfo->pw_gid, groups, &n);
        if (nr == n) {
            for (i=0; i<n; i++) {
                groupinfo = getgrgid(groups[i]);
                printf("    %s (%ld)\n", groupinfo->gr_name, groups[i]);
            }
        }
    }

    return 0;
}

