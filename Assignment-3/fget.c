// Kaustav Vats (2016048)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include "helper.h"

int main(int argc, char const *argv[]) {
    
    if (argc != 2) {
        printf("Usage: fget <filename>\nEx: fget testfile.txt\n");
        seteuid(getuid());
        PrintUserDetails();
        exit(1);
    }
    PrintUserDetails();
    
    if (checkPath(argv[1]) == -1) {
        printf("Error: Path\n");
        seteuid(getuid());
        PrintUserDetails();
        exit(1);
    }

    int permissions = checkFilePermissions(argv[1]);
    if (permissions != 0 && permissions != 2) {
        printf("Error: No permission to read the file!\n");      
        seteuid(getuid());
        PrintUserDetails();  
        exit(1);
    }
    // printf("Permissions: %d\n", permissions);

    int fp;
    char buf[1024];
    memset(buf, '\0', sizeof(buf));

    int len = 0;

    fp = open(argv[1], O_RDONLY);
    if (fp < 0) {
        perror("File Doesn't exist");
        exit(1);
    }
    while ((len = read(fp, &buf, sizeof(buf))) > 0) {
        // printf("%zu\n", read);
        printf("%s\n", buf);
    }

    close(fp);

    seteuid(getuid());
    PrintUserDetails();
    return 0;
}
