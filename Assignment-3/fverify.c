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

    if (argc != 2) {
        printf("Usage: fverify <filename>\nEx: fverify testfile.txt\n");
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

    struct stat sb;
    if (stat(argv[1], &sb) == -1) {
        perror("stat");
        return;
    }

    int ret;
    ret = HMAC_Verify(argv[1], sb.st_uid);
    if (ret > 0) {
        printf("HMAC Correctly Verified!\n");
    }
    else {
        printf("HMAC Error\n");
    }

    seteuid(getuid());
    PrintUserDetails();
    
    return 0;
}
