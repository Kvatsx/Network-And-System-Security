// Kaustav Vats (2016048)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include "helper.h"

int main(int argc, char const *argv[]) {
    
    if (argc < 3) {
        printf("Usage: fput <filename> <strings> \nEx: fput testfile.txt Hey i'm Kaustav, who are you?\n");
        exit(1);
    }
    PrintUserDetails();

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
            exit(1);
        }
    }
    else {
        flag = 1;
    }

    FILE *fptr;

    fptr = fopen(argv[1], "a");
    if(fptr == NULL) {
        printf("Error: Opening File!\n");
        exit(1);
    }
    int i;
    for (i=2; i<argc; i++) {
        fprintf(fptr,"%s", argv[i]);
        fprintf(fptr,"%s", " ");
    }
    fprintf(fptr,"%s", "\n");
    fclose(fptr);

    // Add ACL entries
    if (flag == 1) {
        // char * owner = NULL;
        // char * group = NULL;
        char owner[1024];
        char group[1024];
        memset(owner, '\0', sizeof(owner));
        memset(group, '\0', sizeof(group));

        printf("Enter Owner: ");
        scanf("%s", owner);
        printf("Enter Group: ");
        scanf("%s", group);

        if (changeOwnerGroup(argv[1], owner, group) == -1) {
            perror("chown");
        }
    }

    PrintUserDetails();

    return 0;
}
