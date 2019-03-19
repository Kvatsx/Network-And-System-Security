#include "helper.h"
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <time.h>
#include <sys/xattr.h>

int checkCurrentUser(char * filename, int type) {
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
    char acl_check[1024];
    memset(acl_check, '\0', sizeof(acl_check));
    strcpy(acl_check, "user.");
    strcat(acl_check, pwd->pw_name);

    size = getxattr(filename, acl_check, NULL, 0);
    if ((size = getxattr(filename, acl_check, value, size)) != -1) {
        int num = getPermission(value);
        if ( type == 0 ) {
            if ( num == 0 || num == 2 ) {
                return 1;
            }
        }
        else {
            if ( num == 1 || num == 2 ) {
                return 1;
            }
        }
    }
    if ((sb.st_uid == calling_user && (S_IRUSR & sb.st_mode)) || calling_user == 0) {
        return 1;
    }
    else
    {
        return -1;
    }   
}
// char * SlashParser(char * brr) {
//     char arr[1024];
//     memset(arr, '\0', sizeof(arr));
//     strcpy(arr, brr);
//     char crr[1024];
//     memset(crr, '\0', sizeof(crr));
//     strcpy(crr, brr);
//     printf("Token: %s\n", arr); 
//     int count = 0;
//     int k;
//     for (k=0; k<strlen(arr); k++) {
//         if ( k != 0 && arr[k] == '/' ) {
//             count++;
//         }
//     }
//     printf("Count: %d\n", count);
//     char * token = strtok(crr, "/");
//     int j;
//     for (j=0; j<count; j++) {
//         token = strtok(NULL, "/");                   
//         printf("After %s\n", token);
//     }
//     // printf("tkon: %s\n", token);
//     return token;
// }

void showAclList(char * filename) {
    ssize_t size_acl, size, temp_size, keylen;
    char * buf;
    char * key;
    struct stat sb;
    // char cwd[1024];
    // getcwd(cwd, sizeof(cwd));
    // strcat(cwd, "/");
    // strcat(cwd, filename);
    // char map[1024];
    // realpath(cwd, map);
    // char * end = SlashParser(map);

    if (stat(filename, &sb) == -1) {
        perror("stat:");
        return -1;
    }
    uid_t uid = getuid();
    struct passwd * pwd = getpwuid(sb.st_uid);
    gid_t gid = getgid();
    struct group * gwd = getgrgid(sb.st_gid);

    char val[1024];
    // size = getxattr(filename, "user.filename", NULL, 0);  
    // memset(val, '\0', sizeof(val));
    // if ((size = getxattr(filename, "user.filename", val, size)) != -1) {
    //     printf("# File: %s\n", val);
    // }
    // else {
        printf("# File: %s\n", filename);
    // }

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
                if ( strcmp(key, "user.filename") != 0 ) {
                    printf("%s: %s\n", key, val);                
                }
            }
            keylen = strlen(key) + 1;
            size_acl -= keylen;
            key += keylen;
        }
        
    }
}
// void ActualPath(char * filename, char * fp) {
//     char path[1024];
//     memset(path, '\0', sizeof(path));
//     getcwd(path, sizeof(path));
//     strcat(path, "/");
//     strcat(path, filename);
//     printf("%s\n", path);
//     char temp[1024];
//     realpath(path, temp);
//     sprintf(fp, "%s\0", temp);
// }


// Update Below Function Before Submitting
int checkPath(char * filename) {
    // char path[1024];
    // memset(path, '\0', sizeof(path));

    // char * ret = realpath(filename, path);
    // // printf("%s\n", path);
    // if (strstr(path, "/fakeslash/fakehome") == NULL) {
    //     return -1;
    // }
    return 1;
}

void showFileDetails(char * filename, char * actual) {
    struct stat sb;
    // char path[1024];
    // char Path2[1024];
    // memset(path, '\0', sizeof(path));
    // memset(Path2, '\0', sizeof(Path2));
    // getcwd(Path2, sizeof(Path2));
    // strcat(Path2, "/");
    // strcat(Path2, filename);

    // realpath(Path2, path);
    // printf("RealPath: %s\n", path);

    if (stat(filename, &sb) == -1) {
        perror("stat");
        return;
    }

    struct passwd * pwd = getpwuid(sb.st_uid);
    struct group * gup = getgrgid(sb.st_gid);
    printf("Filename: %s\n", actual);
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
    struct passwd * pwd1 = getpwuid(uid);
    printf("----------------------\nReal user: %s\n", pwd1->pw_name);
    uid_t uid_2 = geteuid();
    struct passwd * pwd2 = getpwuid(uid_2);
    printf("Effective user: %s\n----------------------\n", pwd2->pw_name);

    // printf("%d %d\n", uid, uid_2);

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
    // printf("uid %d\n", uid);

    // printf("filename: %s\n", filename);
    struct stat sb;
    if (stat(filename, &sb) == -1) {
        perror("stat");
        return;
    }
    // printf("uid %d\n", sb.st_uid);

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
        // printf("%s %s\n", pwd_owner->pw_name, pwd->pw_name);
        if ( uid == sb.st_uid ) {
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
        if ( gid == sb.st_gid ) {
            if ((sb.st_mode & S_IRGRP) && (sb.st_mode & S_IWGRP)) {
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

int changeOwnerGroup(char * filename) {
    // owner = strtok(owner, "\n");
    // group = strtok(group, "\n");
    struct passwd * pwd = getpwuid(getuid());
    struct group * gwd = getgrgid(pwd->pw_gid);
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
int giveDir(char * path) {
    int count = 0;
    int flag = 0;
    int i;
    for(i=0; i<strlen(path); i++) {
        if ( path[i] == '/') {
            count = 0;
        }
        count++;
        // printf("%d\n", count);
    }
    return count;
}

int checkFolderPermission(char * filename, char * original) {
    char buf[1024];
    memset(buf, '\0', sizeof(buf));
    int count = giveDir(filename);
    // printf("%d\n", count);
    strncpy(buf, filename, strlen(filename)-count);
    // strcat(buf, "/../");
    // struct stat sb;
    // if (stat(buf, &sb) == -1) {
    //     perror("stat");
    //     return;
    // }
    // printf("buf: %s\n", buf);
    char rel[1024];
    realpath(buf, rel);
    int per = checkFilePermissions(rel);
    // printf("%d\n", per);
    return per;
}
