// Kaustav Vats (2016048)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include "helper.h"
#include <openssl/evp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        printf("Usage: fsign <filename> \nEx: fsign testfile.txt\n");
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
        printf("Usage: fsgin <filename> <strings> \nEx: fsign testfile.txt Hey i'm Kaustav, who are you?\n");
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

    fptr = open(argv[1], O_RDONLY);
    if(fptr < 0) {
        printf("Error: Opening File!\n");
        exit(1);
    }
    char Input[6500];
    char buf[1024];
    memset(Input, '\0', sizeof(Input));
    memset(buf, '\0', sizeof(buf));
    int len = 0;
    while ((len = read(fptr, &buf, sizeof(buf))) > 0) {
        strcat(Input, buf);
    }
    close(fptr);

    struct stat sb;
    if (stat(argv[1], &sb) == -1) {
        perror("stat");
        return;
    }
    create_Hmac(Input, sb.st_uid, argv[1]);

    seteuid(getuid());
    PrintUserDetails();
    return 0;
}
