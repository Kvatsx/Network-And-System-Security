// Kaustav Vats (2016048)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include "helper.h"

int main(int argc, char const *argv[]) {
    
    if (argc < 3) {
        printf("Usage: fput <filename> <strings> \nEx: fput testfile.txt Hey i'm Kaustav, who are you?\n");
        exit(1);
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
        seteuid(getuid());
        PrintUserDetails();
        exit(1);
    }

    char *p;
    p = argv[1];
    if ( p[0] == '-' ) {
        printf("Usage: fput <filename> <strings> \nEx: fput testfile.txt Hey i'm Kaustav, who are you?\n");
        exit(1);
    }

    int flag = 0;
    if ( access(argv[1], F_OK) != -1 ) {
        int permissions = checkFilePermissions(argv[1]);
        if (permissions != 1 && permissions != 2) {
            printf("Error: No permission to read the file!\n");        
            seteuid(getuid());
            PrintUserDetails();
            exit(1);
        }
    }
    else {
        flag = 1;
    }
    
    int fptr;

    fptr = open(argv[1], O_WRONLY | O_APPEND | O_CREAT);
    if(fptr < 0) {
        printf("Error: Opening File!\n");
        exit(1);
    }
    int i;
    char buf[1024];
    memset(buf, '\0', sizeof(buf));
    strcpy(buf, argv[2]);
    for (i=3; i<argc; i++) {
        strcat(buf, " ");
        strcat(buf, argv[i]);
    }
    strcat(buf, "\n");
    write(fptr, buf, strlen(buf));
    close(fptr);

    // Add ACL entries
    if (flag == 1) {
        // char * owner = NULL;
        // char * group = NULL;
        // char owner[1024];
        // char group[1024];
        // memset(owner, '\0', sizeof(owner));
        // memset(group, '\0', sizeof(group));

        // printf("Enter Owner: ");
        // gets(owner);

        // printf("Enter Group: ");
        // gets(group);

        if (changeOwnerGroup(argv[1]) == -1) {
            perror("chown");
        }
    }

    seteuid(getuid());
    PrintUserDetails();

    return 0;
}
