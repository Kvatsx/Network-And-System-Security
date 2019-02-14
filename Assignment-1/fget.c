// Kaustav Vats (2016048)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "helper.h"

int main(int argc, char const *argv[]) {
    
    if (argc != 2) {
        printf("Usage: fget <filename>\nEx: fget testfile.txt\n");
        exit(1);
    }
    PrintUserDetails();
    
    if (checkPath(argv[1]) == -1) {
        printf("Error: Path\n");
        exit(1);
    }

    int permissions = checkFilePermissions(argv[1]);
    if (permissions != 0 && permissions != 2) {
        printf("Error: No permission to read the file!\n");        
        exit(1);
    }
    // printf("Permissions: %d\n", permissions);

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("File Doesn't exist");
        exit(1);
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("%zu\n", read);
        printf("%s", line);
    }

    fclose(fp);
    if (line) {
        free(line);
    }

    seteuid(getuid());
    PrintUserDetails();
    return 0;
}
