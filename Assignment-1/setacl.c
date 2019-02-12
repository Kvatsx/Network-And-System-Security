// Kaustav Vats (2016048)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <sys/xattr.h>
#include "helper.h"


int main(int argc, char const *argv[]) {

    if ( argc != 4 ) {
        printf("Usage: setacl <user.name/user.group.name> <permissions> <filename>\nEx: setacl user.thor r-x testfile.txt\n");
        exit(1);
    }
    // printf("Filename: %s\n", argv[3]);
    if (checkCurrentUser(argv[3]) == -1) {
        perror("Owner mismatch:");   
        return -1;
    }
    else {
        char temp[1024];
        memset(temp, '\0', sizeof(temp));
        strcpy(temp, argv[1]);
        char * token = strtok(temp, ".");
        token = strtok(NULL, ".");
        printf("name: %s\n%d\n", token, strlen(token));
        struct passwd * pwd = getpwnam(token);

        if (pwd != NULL) {
            if ( setxattr(argv[3], argv[1], argv[2], strlen(argv[2]), 0) == -1) {
                // printf("%s\n", pwd->pw_name);
                printf("%s\n", argv[1]);
                printf("%s\n", argv[2]);
                printf("%s\n", argv[3]);
                perror("setxattr: ");   
                exit(1);
            }
        }
        if (pwd == NULL) {
            printf("Error: User doesn't exist!\n");
        }
    }

    return 0;
}

