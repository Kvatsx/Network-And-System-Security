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
    
    if ((sb.st_uid == calling_user && (S_IRUSR & sb.st_mode)) || calling_user == 0) {
        return 1;
    }
    else
    {
        return -1;
    }   
}

void showAclList(char * filename) {
    ssize_t size_acl, size, temp_size, keylen;
    char * buf;
    char * key;
    struct stat sb;

    if (stat(filename, &sb) == -1) {
        perror("stat:");
        return -1;
    }
    uid_t uid = getuid();
    struct passwd * pwd = getpwuid(uid);
    gid_t gid = getgid();
    struct group * gwd = getgrgid(gid);

    printf("# File: %s\n", filename);
    printf("# Owner: %s\n", pwd->pw_name);
    printf("# Group: %s\n", gwd->gr_name);
    printf("# Permission: ");
    printf( (sb.st_mode & S_IRUSR) ? "r" : "-");
    printf( (sb.st_mode & S_IWUSR) ? "w" : "-");
    printf( (sb.st_mode & S_IXUSR) ? "x" : "-");
    printf( (sb.st_mode & S_IRGRP) ? "r" : "-");
    printf( (sb.st_mode & S_IWGRP) ? "w" : "-");
    printf( (sb.st_mode & S_IXGRP) ? "x" : "-");
    printf( (sb.st_mode & S_IROTH) ? "r" : "-");
    printf( (sb.st_mode & S_IWOTH) ? "w" : "-");
    printf( (sb.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");


    char val[1024];
    memset(val, '\0', sizeof(val));
    size_acl = listxattr(filename, NULL, 0);

    buf = malloc(size_acl);
    if ((size_acl = listxattr(filename, buf, size_acl)) != -1) {
        key = buf;

        while(size_acl > 0){
            // printf("Key: %s\n", key);
            size = getxattr(filename, key, NULL, 0);  
            memset(val, '\0', sizeof(val));
            if ((size = getxattr(filename, key, val, size)) != -1) {
                printf("%s: %s\n", key, val);                
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

    struct passwd * pwd = getpwuid(sb.st_uid);
    struct group * gup = getgrgid(sb.st_gid);
    printf("Filename: %s\n", filename);
    printf("Owner: %s\n", pwd->pw_name); // Need to change to ACL check first.
    printf("Group: %s\n", gup->gr_name);
    printf("Permissions: ");
    printf( (sb.st_mode & S_IRUSR) ? "r" : "-");
    printf( (sb.st_mode & S_IWUSR) ? "w" : "-");
    printf( (sb.st_mode & S_IXUSR) ? "x" : "-");
    printf( (sb.st_mode & S_IRGRP) ? "r" : "-");
    printf( (sb.st_mode & S_IWGRP) ? "w" : "-");
    printf( (sb.st_mode & S_IXGRP) ? "x" : "-");
    printf( (sb.st_mode & S_IROTH) ? "r" : "-");
    printf( (sb.st_mode & S_IWOTH) ? "w" : "-");
    printf( (sb.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");
    printf("File Size: %lld\n", sb.st_size);
    printf("Last File Modification: %s\n", ctime(&sb.st_mtime));
}

void PrintUserDetails(void) {
    uid_t uid = getuid();
    uid_t uid_2 = geteuid();
    struct passwd * pwd1 = getpwuid(uid);
    struct passwd * pwd2 = getpwuid(uid_2);

    printf("----------------------\nReal user: %s\n", pwd1->pw_name);
    printf("Effective user: %s\n----------------------\n", pwd2->pw_name);
}

int getPermission(char * permission) {
    if (permission[0] == 'r' && permission[1] == 'w') {
        return 2;
    }
    else if (permission[1] == 'w') {
        return 1;
    }
    else if (permission[0] == 'r') {
        return 0;
    }
    else {
        return -1;
    }
}


int checkFilePermissions(char * filename) {
    uid_t uid = getuid();
    struct passwd * pwd = getpwuid(uid);
    gid_t gid = getgid();
    struct group * gwd = getgrgid(gid);

    struct stat sb;
    if (stat(filename, &sb) == -1) {
        perror("stat");
        return;
    }
    struct passwd * pwd_owner = getpwuid(sb.st_uid);
    struct group * gwd_owner = getgrgid(sb.st_gid);

    char key[1024];
    memset(key, '\0', sizeof(key));
    strcpy(key, "user.");
    strcat(key, pwd->pw_name);
    // printf("Key: %s\n", key);

    char value[1024];
    memset(value, '\0', sizeof(value));
    // Checking for file permissions
    ssize_t size;
    size = getxattr(filename, key, NULL, 0);
    if ((size = getxattr(filename, key, value, size)) == -1) {
        // if Name not found in ACL then check for owner and Group.
        // printf("LOL\n");
        if ( strcmp(pwd_owner->pw_name, pwd->pw_name) == 0 ) {
            if ((sb.st_mode & S_IRUSR) && (sb.st_mode &  S_IWUSR)) {
                return 2;
            }
            else if ( sb.st_mode & S_IRUSR ) {
                return 0;
            }
            else if ( sb.st_mode &  S_IWUSR ) {
                return 1;
            }
        }
        // printf("LOL2\n");

        // Check for group permissons
        if ( strcmp(gwd_owner->gr_name, gwd->gr_name) == 0 ) {
            if ((sb.st_mode & S_IRGRP) && (sb.st_mode &  S_IWGRP)) {
                return 2;
            }
            else if ( sb.st_mode & S_IRGRP ) {
                return 0;
            }
            else if ( sb.st_mode &  S_IWGRP ) {
                return 1;
            }
        }
        // Check Others permissions
        if ((sb.st_mode & S_IROTH) && (sb.st_mode &  S_IWOTH)) {
            return 2;
        }
        else if ( sb.st_mode & S_IROTH ) {
            return 0;
        }
        else if ( sb.st_mode &  S_IWOTH ) {
            return 1;
        }
        
    }
    else {
        int permissions = getPermission(value);
        return permissions;
    }
    return -1;
}

int AddAclEntry(char * filename, char * owner, char * group) {
    uid_t uid = getuid();
    struct passwd * pwd = getpwuid(uid);
    gid_t gid = getgid();
    struct group * gwd = getgrgid(gid);

    if (strcmp(owner, "\n") == 0  && strcmp(group, "\n") == 0) {
        printf("Default Permissions!\n");
        
    }
    return 0;
}

int changeOwnerGroup(char * filename, char * owner, char * group) {
    // owner = strtok(owner, "\n");
    // group = strtok(group, "\n");

    struct passwd * pwd = getpwnam(owner);
    struct group * gwd = getgrnam(group);
    // printf("owner: %s\ngroup: %s\n", owner, group);
    // printf("%s %s\n", pwd->pw_name, gwd->gr_name);
    if ( pwd != NULL && gwd != NULL ) {
        int ret = chown(filename, pwd->pw_uid, gwd->gr_gid);
        if ( ret == -1 ) {
            perror("chown");
        }
        return ret;
    }
    return -1;
}
