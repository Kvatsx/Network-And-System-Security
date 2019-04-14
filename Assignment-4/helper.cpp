/*
 * @Author: Kaustav Vats 
 * @Roll-Number: 2016048 
 */

#include "helper.h"

using namespace std;

int showFileDetails(char * filename, char * actual) {
    struct stat sb;
    if (stat(filename, &sb) == -1) {
        perror("stat");
        return -1;
    }

    struct passwd * pwd = getpwuid(sb.st_uid);
    struct group * gup = getgrgid(sb.st_gid);
    printf("Filename: %s\n", actual);

    return 1;
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


int checkFilePermissions(const char * filename, string user) {
    struct passwd * pwd = getpwnam(user.c_str());
    uid_t uid = pwd->pw_uid;
    gid_t gid = pwd->pw_gid;

    char key[1024];
    memset(key, '\0', sizeof(key));
    strcpy(key, "user.");
    strcat(key, user.c_str());
    // printf("Key: %s\n", key);

    printf("filename: %s\n", filename);
    struct stat sb;
    if (stat(filename, &sb) == -1) {
        perror("stat");
        return -1;
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

int do_enc(const unsigned char * input, string username, int encdec, unsigned char * out) {
    unsigned char *key;
    unsigned char *iv;

    key = (unsigned char *) malloc(sizeof(unsigned char) * 32);   
    iv = (unsigned char *) malloc(sizeof(unsigned char) * 16); 

    getKeyIv(username, key, iv);

    int outlen, tmplen;

    EVP_CIPHER_CTX *ctx;

    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    if(!EVP_EncryptUpdate(ctx, out, &outlen, input, strlen((const char *) input)))
    {
        return 0;
    }

    if(!EVP_EncryptFinal_ex(ctx, out + outlen, &tmplen))
    {
        return 0;
    }
    outlen += tmplen;
    EVP_CIPHER_CTX_free(ctx);
    // out[outlen] = "\0";
    printf("OUTPUT: %s\n", out);
    return 1;
}

int do_dec(const unsigned char * input, string username, int encdec, unsigned char * out) {
    unsigned char *key;
    unsigned char *iv;

    key = (unsigned char *) malloc(sizeof(unsigned char) * 32);   
    iv = (unsigned char *) malloc(sizeof(unsigned char) * 16); 

    getKeyIv(username, key, iv);

    int outlen, tmplen;

    EVP_CIPHER_CTX *ctx;

    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    if(!EVP_DecryptUpdate(ctx, out, &outlen, input, strlen((const char *)input)))
    {
        perror("dec update");
        return 0;
    }

    // if(!EVP_DecryptFinal_ex(ctx,(unsigned char *) (out + outlen), &tmplen))
    // {
    //     perror("Dec Final");
    //     return 0;
    // }
    cout << "Outlen: " << outlen << endl;
    printf("OUTPUT: %s\n", out);
    // outlen += tmplen;
    // out[outlen] = reinterpret_cast<const unsigned char *>( "\0") ;
    EVP_CIPHER_CTX_free(ctx);
    return outlen;
}
void getKeyIv(string username, unsigned char * key, unsigned char * iv) {
    struct passwd * pwd = getpwnam(username.c_str());
    
    FILE * fd;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fd = fopen("/etc/shadow", "r");
    if (fd == NULL) {
        printf("File could not be opened.\n");
        perror("open");
        return;
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
        }
    }
    fclose(fd);
}

void do_crypt(const unsigned char * input, string username, int encdec, unsigned char * out) {
    unsigned char *key;
    unsigned char *iv;

    key = (unsigned char *) malloc(sizeof(unsigned char) * 32);   
    iv = (unsigned char *) malloc(sizeof(unsigned char) * 16); 

    getKeyIv(username, key, iv);

    unsigned char output[sizeof(input) + EVP_MAX_BLOCK_LENGTH];
    // memset(output, '\0', strlen(output));
    int input_len, output_len;

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), NULL, NULL, NULL, encdec);
    OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 32);
    OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);
    EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, encdec);

    cout << "do_crypt Called!" << endl;

    input_len = sizeof(input);

    if(!EVP_CipherUpdate(ctx, output, &output_len, input, input_len)) {
        perror("Here");
        EVP_CIPHER_CTX_free(ctx);
        return;
    }
    cout << "output_len " << strlen((const char *)output) << endl;
    strcpy((char *)out, (const char *)output);

    if(!EVP_CipherFinal_ex(ctx, output, &output_len)) {
        perror("where");
        EVP_CIPHER_CTX_free(ctx);
        return;
    }   

    EVP_CIPHER_CTX_free(ctx);
}
// void String2Char(string input, char * output) {

// }
