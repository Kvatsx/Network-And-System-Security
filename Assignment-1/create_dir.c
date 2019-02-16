// Kaustav Vats (2016048)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <limits.h>
#include <stdlib.h>

void Error() {
	fprintf(stderr, "Cannot create directory\n");
	exit(1);
}
void SyntaxError(const char *in) {
	fprintf(stderr, "Syntax Error: %s\nUse: create_dir [name]\nEx: mkdir foo\n", in);
	exit(1);
}

int main(int argc, char const *argv[]) {
   
    if ( argc != 2 ) {
        SyntaxError(argv[0]);
    }
    PrintUserDetails();

    if (checkPath(argv[1]) == -1) {
        printf("Error: Path\n");
        exit(1);
    }
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, "/");
    strcat(cwd, argv[1]);
    char rp[1024];
    realpath(cwd, rp);
    printf("Path: %s\n", rp);

    int ret = checkFolderPermission(rp, argv[1]);
    if (ret == -1 || ret == 0) {
        printf("Error: Directory permissions\n");
        exit(1);
    }

    char *p;
    p = argv[1];
    if ( p[0] == '-' ) {
        SyntaxError(argv[0]);
    }
    else if ( mkdir(argv[1], S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP) < 0 ) {
        Error();
    }
    else {
        // char owner[1024];
        // char group[1024];
        // memset(owner, '\0', sizeof(owner));
        // memset(group, '\0', sizeof(group));

        // printf("Enter Owner: ");
        // gets(owner);

        // printf("Enter Group: ");
        // gets(group);
        
        if (changeOwnerGroup(argv[1]) == -1) {
            printf("Error: Owner and Group not set\nDefault permissions given!\n");
        }
    }

    seteuid(getuid());
    PrintUserDetails();

    return 0;
}
