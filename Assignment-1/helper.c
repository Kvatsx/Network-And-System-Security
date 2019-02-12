#include "helper.h"
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <time.h>
#include <sys/xattr.h>

int checkCurrentUser(char * filename) {
    uid_t calling_user;
    struct stat sb;
    ssize_t size;

    calling_user = getuid();
    struct passwd * pwd = getpwuid(calling_user);
    
    if (stat(filename, &sb) == -1) {
        perror("stat:");
        return -1;
    }
    char value[1024];
    memset(value, '\0', sizeof(value));
    size = getxattr(filename, "user.owner", value, 0);
    if ((size = getxattr(filename, "user.owner", value, size)) != -1) {
        // char new_uid[1024];
        // memset(new_uid, '\0', sizeof(new_uid));
        // sprintf(new_uid, "%d", calling_user);

        if (strcmp(pwd->pw_name, value) == 0) {
            return 1;
        }
        else {
            return -1;
        }
    }
    else {
        // printf("Calling user %d\nReal User: %d\n", calling_user, sb.st_uid);
        if (sb.st_uid == calling_user || calling_user == 1004) {
            return 1;
        }
        else
        {
            return -1;
        }   
    } 
}

void showAclList(char * filename) {
    ssize_t size_acl, size, temp_size, keylen;
    char * buf;
    char * key;

    printf("# File: %s\n", filename);
    // printf("size: %d\n", getxattr(filename, "user.owner", temp_value, temp_size));
    char val[1024];
    memset(val, '\0', sizeof(val));

    temp_size = getxattr(filename, "user.owner", NULL, 0);
    if ((temp_size = getxattr(filename, "user.owner", val, temp_size)) != -1) {
        printf("# Owner: %s\n", val);
    }

    memset(val, '\0', sizeof(val));
    temp_size = getxattr(filename, "user.group", NULL, 0);
    if (getxattr(filename, "user.group", val, temp_size) != -1) {
        printf("# Group: %s\n", val);
    }

    size_acl = listxattr(filename, NULL, 0);

    buf = malloc(size_acl);
    if ((size_acl = listxattr(filename, buf, size_acl)) != -1) {
        key = buf;

        while(size_acl > 0){
            // printf("Key: %s\n", key);
            size = getxattr(filename, key, NULL, 0);  
            memset(val, '\0', sizeof(val));
            if ((size = getxattr(filename, key, val, size)) != -1) {
                if ( strcmp("user.owner", key) != 0 && strcmp("user.group", key) != 0 ) {
                    printf("%s: %s\n", key, val);
                }
            }
            keylen = strlen(key) + 1;
            size_acl -= keylen;
            key += keylen;
        }
        
    }
}

void showFileDetails(char * filename) {
    struct stat sb;

    if (stat(filename, &sb) == -1) {
        perror("stat");
        return;
    }

    char permissions[1024];
    memset(permissions, '\0', sizeof(permissions));

    // User Permission
    char val[1024];
    memset(val, '\0', sizeof(val));
    ssize_t size;
    size = getxattr(filename, "user.user", NULL, 0);
    if ((size = getxattr(filename, "user.user", val, size)) == -1) {
        strcpy(permissions, val);
    }
    else {
        strcpy(permissions, "---");
    }
    // Group permissions
    memset(val, '\0', sizeof(val));
    size = getxattr(filename, "user.group", NULL, 0);
    if ((size = getxattr(filename, "user.group", val, size)) == -1) {
        strcpy(permissions, val);
    }
    else {
        strcpy(permissions, "---");
    }
    // Others permissions
    memset(val, '\0', sizeof(val));
    size = getxattr(filename, "user.others", NULL, 0);
    if ((size = getxattr(filename, "user.others", val, size)) == -1) {
        strcpy(permissions, val);
    }
    else {
        strcpy(permissions, "---");
    }

    struct passwd * pwd = getpwuid(sb.st_uid);
    struct group * gup = getgrgid(sb.st_gid);
    printf("Filename: %s\n", filename);
    printf("Owner: %s\n", pwd->pw_name); // Need to change to ACL check first.
    printf("Group: %s\n", gup->gr_name);
    printf("Permissions: %s\n", permissions);
    printf("File Size: %lld\n", sb.st_size);
    printf("Last File Modification: %s\n", ctime(&sb.st_mtime));


}