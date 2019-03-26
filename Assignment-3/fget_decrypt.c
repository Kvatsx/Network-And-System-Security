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
        printf("Usage: fget_decrypt <filename>\nEx: fget testfile.txt\n");
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
    char Input[6500];
    int fp;
    char buf[1024];
    memset(buf, '\0', sizeof(buf));
    memset(Input, '\0', sizeof(Input));

    int len = 0;

    fp = open(argv[1], O_RDONLY);
    if (fp < 0) {
        perror("File Doesn't exist");
        exit(1);
    }
    while ((len = read(fp, &buf, sizeof(buf))) > 0) {
        // printf("%zu\n", read);
        strcat(Input, buf);
    }

    close(fp);

    // ----------------------------------------------------------

    struct stat sb;
    if (stat(argv[1], &sb) == -1) {
        perror("stat");
        return;
    }

    if (HMAC_Verify(argv[1], sb.st_uid) < 0) {
        printf("HMAC: Error\n");
        seteuid(getuid());
        PrintUserDetails();        
        return 0;
    }
    printf("HMAC: Correctly Matched!\n");
 
    struct passwd * pwd_owner = getpwuid(sb.st_uid);
    // printf("User: %s\n", pwd_owner->pw_name);
    unsigned char *out;
    out = (unsigned char *) malloc(sizeof(unsigned char) * (strlen(buf)+1));
    memset(out, "\0", sizeof(out));
    do_dec(Input, sb.st_uid, 0, out);
    printf("%s\n", out);

    // ----------------------------------------------------------
    
    seteuid(getuid());
    PrintUserDetails();
    return 0;
}
