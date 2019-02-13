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
    int flag = 0;
    if ( access(argv[1], F_OK) != -1 ) {
        int permissions;
        if ((permissions = checkFilePermissions(argv[1])) == -1) {
            printf("Error: Permissions\n");
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
        char * owner = NULL;
        char * group = NULL;
        size_t size;
        printf("Enter Owner: ");
        if (getline(&owner, &size, stdin) == -1) {
            printf("Input Error!\n");
        }
        printf("Enter Group: ");
        if (getline(&group, &size, stdin) == -1) {
            printf("Input Error!\n");
        }

        int ret = AddAclEntry(argv[1], owner, group);
        
    }

    return 0;
}
