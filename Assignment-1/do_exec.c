// Kaustav Vats (2016048)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>

int main(int argc, char const *argv[]) {

    if (argc != 2) {
        printf("Usage: do_exec <binary>\nEx: do_exec MyBin\n");
        exit(1);
    }
    PrintUserDetails();

    uid_t RealUser = getuid();

    struct stat sb;
    if (stat(argv[1], &sb) == -1) {
        perror("stat");
        return;
    }
    
    uid_t owner = sb.st_uid;

    char *temp[argc];
    int i;
    int index = 0;
    for  (i=1; i<argc; i++) {
        temp[index] = argv[i];
        index++;
    }
    temp[argc-1] = NULL;

    if (fork() == 0) {
        setuid(owner);
        if (execvp(argv[1], temp) < 0) {
            perror("Execvp");
        }
        setuid(RealUser);
    }
    else {
        wait(NULL);
    }

    PrintUserDetails();

    return 0;
}
