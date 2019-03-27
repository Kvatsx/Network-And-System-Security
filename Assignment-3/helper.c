#include "helper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <time.h>
#include <sys/xattr.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

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
    printf("per: %s\n", permission);
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
    // printf("uid %s\n", pwd->pw_name);

    char key[1024];
    memset(key, '\0', sizeof(key));
    strcpy(key, "user.");
    strcat(key, pwd->pw_name);
    // printf("Key: %s\n", key);

    printf("filename: %s\n", filename);
    struct stat sb;
    if (stat(filename, &sb) == -1) {
        perror("stat");
        return;
    }
    // printf("uid %d\n", sb.st_uid);

    struct passwd * pwd_owner = getpwuid(sb.st_uid);
    struct group * gwd_owner = getgrgid(sb.st_gid);


    char value[1024];
    memset(value, '\0', sizeof(value));
    // Checking for file permissions
    ssize_t size;
    int permissions = -100;
    size = getxattr(filename, key, NULL, 0);
    if ((size = getxattr(filename, key, value, size)) == -1) {
        // if Name not found in ACL then check for owner and Group.
        // printf("LOL\n");
        // printf("%s %s\n", pwd_owner->pw_name, pwd->pw_name);

        // printf("Not in acl\n");
        if ( uid == sb.st_uid ) {
            if ((sb.st_mode & S_IRUSR) && (sb.st_mode &  S_IWUSR)) {
                if (2 > permissions) {
                    permissions = 2;
                }
            }
            else if ( sb.st_mode & S_IRUSR ) {
                if (0 > permissions) {
                    permissions = 0;
                }
            }
            else if ( sb.st_mode &  S_IWUSR ) {
                if (1 > permissions) {
                    permissions = 1;
                }
            }
        }
        // printf("LOL2\n");

        // Check for group permissons
        if ( gid == sb.st_gid ) {
            if ((sb.st_mode & S_IRGRP) && (sb.st_mode & S_IWGRP)) {
                if (2 > permissions) {
                    permissions = 2;
                }
            }
            else if ( sb.st_mode & S_IRGRP ) {
                if (0 > permissions) {
                    permissions = 0;
                }
            }
            else if ( sb.st_mode &  S_IWGRP ) {
                if (1 > permissions) {
                    permissions = 1;
                }
            }
        }
        // Check Others permissions
        if ((sb.st_mode & S_IROTH) && (sb.st_mode &  S_IWOTH)) {
            if (2 > permissions) {
                permissions = 2;
            }
        }
        else if ( sb.st_mode & S_IROTH ) {
            if (0 > permissions) {
                permissions = 0;
            }
        }
        else if ( sb.st_mode &  S_IWOTH ) {
            if (1 > permissions) {
                permissions = 1;
            }
        }
        
    }
    else {
        permissions = getPermission(value);      
        // printf("returned PEr %d\n", permissions);  
        return permissions;
    }
    return permissions;
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
    printf("USeR: %s\n", pwd->pw_name);
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

// https://www.openssl.org/docs/manmaster/man3/PKCS5_PBKDF2_HMAC_SHA1.html
// https://stackoverflow.com/questions/9771212/how-to-use-pkcs5-pbkdf2-hmac-sha1

void getKeyIv(uid_t uid, unsigned char * key, unsigned char * iv) {
    struct passwd * pwd = getpwuid(uid);
    
    FILE * fd;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fd = fopen("/etc/shadow", "r");
    if (fd == NULL) {
        printf("File could not be opened.\n");
        perror("open");
        return NULL;
    }

    while ((read = getline(&line, &len, fd)) != -1) {

        char * token = strtok(line, ":");
        if (strcmp(token, pwd->pw_name) == 0) {
            token = strtok(NULL, ":");
            int i;

            if( PKCS5_PBKDF2_HMAC_SHA1(token, strlen(token), NULL, 0, 100, 32, key) == 0 ) {
                perror("PBKDF");
            }
            if( PKCS5_PBKDF2_HMAC_SHA1(token, strlen(token), NULL, 0, 50, 16, iv) == 0 ) {
                perror("PBKDF");
            }
            // close(fd);
            // return out;
        }
    }
    close(fd);
    // return NULL;
}

void do_enc(char * input, uid_t uid, int encdec, unsigned char * out) {
    int outlen, tmplen;

    unsigned char *key;
    unsigned char *iv;

    key = (unsigned char *) malloc(sizeof(unsigned char) * 32);   
    iv = (unsigned char *) malloc(sizeof(unsigned char) * 16); 

    getKeyIv(uid, key, iv);

    EVP_CIPHER_CTX *ctx;

    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    if(!EVP_EncryptUpdate(ctx, out, &outlen, input, strlen(input)))
    {
        return 0;
    }

    if(!EVP_EncryptFinal_ex(ctx, out + outlen, &tmplen))
    {
        return 0;
    }
    outlen += tmplen;
    EVP_CIPHER_CTX_free(ctx);
    out[outlen] = "\n";
    printf("OUTPUT: %s\n", out);
}

void do_dec(char * input, uid_t uid, int encdec, unsigned char * out) {
    int outlen, tmplen;

    unsigned char *key;
    unsigned char *iv;

    key = (unsigned char *) malloc(sizeof(unsigned char) * 32);   
    iv = (unsigned char *) malloc(sizeof(unsigned char) * 16); 

    getKeyIv(uid, key, iv);

    EVP_CIPHER_CTX *ctx;

    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    if(!EVP_DecryptUpdate(ctx, out, &outlen, input, strlen(input)))
    {
        perror("dec update");
        return;
    }

    // if(!EVP_DecryptFinal_ex(ctx, out + outlen, &tmplen))
    // {
    //     perror("Dec Final");
    //     return;
    // }

    // printf("OUTPUT: %d\n", outlen);
    // outlen += tmplen;
    // out[outlen] = "\0";
    EVP_CIPHER_CTX_free(ctx);
}

// encdec 1 Encryption, 0 Decryption
unsigned char * do_crypt(char * input, uid_t uid, int encdec, unsigned char * out) {
    unsigned char *key;
    unsigned char *iv;

    key = (unsigned char *) malloc(sizeof(unsigned char) * 32);   
    iv = (unsigned char *) malloc(sizeof(unsigned char) * 16); 

    getKeyIv(uid, key, iv);
    // iv = getKey(uid);
    // unsigned char iv[] = "1234567887654321";
    printf("Key %s\n%d\n", key, strlen(key));
    printf("Iv %s\n%d\n", iv, strlen(iv));

    unsigned char * output[strlen(input) + EVP_MAX_BLOCK_LENGTH];
    memset(output, '\0', strlen(output));
    int input_len, output_len;

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), NULL, NULL, NULL, encdec);
    OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 32);
    OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);
    EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, encdec);

    if(!EVP_CipherUpdate(ctx, output, &output_len, input, input_len)) {
        perror("Here");
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    strcpy(out, output);

    // if(!EVP_CipherFinal_ex(ctx, output, &output_len)) {
    //     perror("where");
    //     EVP_CIPHER_CTX_free(ctx);
    //     return NULL;
    // }

    EVP_CIPHER_CTX_free(ctx);

    return output;

}

// http://www.askyb.com/cpp/openssl-hmac-hasing-example-in-cpp/
// https://www.openssl.org/docs/man1.1.0/man3/HMAC.html
void create_Hmac(char * input, uid_t uid, char * filename) {
    unsigned char *key;
    unsigned char *iv;

    key = (unsigned char *) malloc(sizeof(unsigned char) * 32);   
    iv = (unsigned char *) malloc(sizeof(unsigned char) * 16); 
    printf("Nano\n");
    // key = getKey(uid);
    getKeyIv(uid, key, iv);
    printf("BAM\n%d\n", strlen(key));
    // iv = getKey(uid);
    // unsigned char iv[] = "1234567887654321";
    // printf("Key %s\n", key);
    // printf("Iv %s\n", iv);

    unsigned char* result;
    unsigned int len = 20;
    // result = (unsigned char*)malloc(sizeof(char) * EVP_MAX_MD_SIZE);

    // HMAC_CTX *ctx;
    // ctx = HMAC_CTX_new();
    // HMAC_CTX_init(&ctx);

    // HMAC_Init_ex(&ctx, key, strlen(key), EVP_sha1(), NULL);
    // HMAC_Update(&ctx, (unsigned char*)&input, strlen(input));
    // HMAC_Final(&ctx, result, &len);
    // HMAC_CTX_cleanup(&ctx);

    result = HMAC(EVP_sha1(), key, strlen(key), input, strlen(input), NULL, NULL);
    // result = HMAC(EVP_sha1(), key, strlen(key), input, strlen(input), result, sizeof(result));
    printf("Res: %s\n", result);

    char newFile[5000];
    memset(newFile, "\0", sizeof(newFile));
    strcpy(newFile, filename);
    strcat(newFile, ".sign");

    // FILE *fptr;

    // fptr = fopen(newFile, "w+");
    // if(fptr == NULL)
    // {
    //     printf("Error!");
    //     exit(1);
    // }

    // fprintf(fptr,"%s", result);
    // // fprintf(fptr,"%s", "\n");
    // fclose(fptr);

    int fptr;
    fptr = open(newFile, O_WRONLY | O_CREAT | O_TRUNC, 644);
    if(fptr < 0) {
        printf("Error: Opening File!\n");
        exit(1);
    }
    write(fptr, result, strlen(result));
    close(fptr);
    if (changeOwnerGroup(newFile) == -1) {
        perror("chown");
    }
    return;
}

int HMAC_Verify(char * Filename, uid_t uid) {
    unsigned char *key;
    unsigned char *iv;

    key = (unsigned char *) malloc(sizeof(unsigned char) * 32);   
    iv = (unsigned char *) malloc(sizeof(unsigned char) * 16); 
    // printf("Nano\n");
    // key = getKey(uid);
    getKeyIv(uid, key, iv);
    // printf("BAM\n%d\n", strlen(key));
    // iv = getKey(uid);
    // unsigned char iv[] = "1234567887654321";
    // printf("Key %s\n", key);
    // printf("Iv %s\n", iv);

    char Input[6500];
    int fp;
    char buf[1024];
    memset(buf, '\0', sizeof(buf));
    memset(Input, '\0', sizeof(Input));
    int len = 0;
    fp = open(Filename, O_RDONLY);
    if (fp < 0) {
        perror("File Doesn't exist");
        exit(1);
    }
    while ((len = read(fp, &buf, sizeof(buf))) > 0) {
        strcat(Input, buf);
    }
    close(fp);

    unsigned char* result;
    // result = (unsigned char*)malloc(sizeof(char) * EVP_MAX_MD_SIZE);

    result = HMAC(EVP_sha1(), key, strlen(key), Input, strlen(Input), NULL, NULL);
    // result = HMAC(EVP_sha1(), key, strlen(key), input, strlen(input), result, sizeof(result));
    // printf("Res: %s\n", result);
    // --------------------------------------
    char newFile[5000];
    memset(newFile, '\0', sizeof(newFile));
    strcpy(newFile, Filename);
    strcat(newFile, ".sign");
    memset(buf, '\0', sizeof(buf));
    memset(Input, '\0', sizeof(Input));
    fp = open(newFile, O_RDONLY);
    if (fp < 0) {
        perror("File Doesn't exist");
        exit(1);
    }
    while ((len = read(fp, &buf, sizeof(buf))) > 0) {
        strcat(Input, buf);
    }
    close(fp);

    if (strcmp(Input, result) == 0) {
        return 1;
    }
    return -1;
}