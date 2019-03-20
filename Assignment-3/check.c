// Kaustav Vats (2016048)

// https://wiki.openssl.org/index.php/EVP_Authenticated_Encryption_and_Decryption
// https://wiki.openssl.org/index.php/EVP_Signing_and_Verifying
// https://stackoverflow.com/questions/9889492/how-to-do-encryption-using-aes-in-openssl
// https://www.openssl.org/docs/manmaster/man3/EVP_EncryptInit.html
// https://www.openssl.org/docs/manmaster/man3/EVP_VerifyInit.html
// https://www.openssl.org/docs/manmaster/man3/EVP_SignInit.html

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>

#include <openssl/evp.h>

#define KEY_LEN      32
#define KEK_KEY_LEN  20
#define ITERATION     1 

unsigned char * getKey(uid_t uid) {
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
        // printf("%s", line);
        char * token = strtok(line, ":");
        // printf("name: %s\nuser: %s\n", token,  pwd->pw_name);
        // printf("User: %s\n", pwd->pw_name);

        if (strcmp(token, pwd->pw_name) == 0) {
            token = strtok(NULL, ":");
            int i;
            unsigned char *out;

            out = (unsigned char *) malloc(sizeof(unsigned char) * 16);

            // printf("pass: %s\n", token);

            if( PKCS5_PBKDF2_HMAC_SHA1(token, strlen(token), NULL, 0, 1, 16, out) != 0 ) {
                // printf("out: "); 
                // for(i=0;i<16;i++) { 
                //     printf("%02x", out[i]); 
                // } 
                // printf("\n");
            }
            else {
                fprintf(stderr, "PKCS5_PBKDF2_HMAC_SHA1 failed\n");
            }
            return out;
        }
    }
    return NULL;
}

// encdec 1 Encryption, 0 Decryption
unsigned char * getEncrypted(char * input, uid_t uid, int encdec, unsigned char * out) {
    unsigned char *key;
    // unsigned char *iv;

    key = (unsigned char *) malloc(sizeof(unsigned char) * 16);   
    // iv = (unsigned char *) malloc(sizeof(unsigned char) * 16); 

    key = getKey(uid);
    // iv = getKey(uid);
    unsigned char iv[] = "1234567887654321";
    // printf("Key %s\n", key);
    // printf("Iv %s\n", iv

    // unsigned char * output;
    unsigned char * output[strlen(input) + EVP_MAX_BLOCK_LENGTH];
    // output = (unsigned char *) malloc(sizeof(unsigned char) * (strlen(input) + EVP_MAX_BLOCK_LENGTH));
    memset(output, '\0', strlen(output));
    int input_len, output_len;

    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    EVP_CipherInit_ex(ctx, EVP_aes_128_cbc(), NULL, NULL, NULL, encdec);

    OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 16);

    OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);

    EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, encdec);

    // printf("YOLO\n");    

    if(!EVP_CipherUpdate(ctx, output, &output_len, input, input_len)) {
        perror("Here");
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    // printf("ALL\n");    

    strcpy(out, output);
    // printf("Res1 %s\n", output);

    // if(!EVP_CipherFinal_ex(ctx, output, &output_len)) {
    //     perror("where");
    //     EVP_CIPHER_CTX_free(ctx);
    //     return NULL;
    // }

    // printf("why2\n");

    EVP_CIPHER_CTX_free(ctx);

    return output;

}

int main(int argc, char const *argv[]) {
    
    // int i;
    unsigned char *out;
    const char pwd[] = "Hello World\n";
    // unsigned char *out[strlen(pwd)+ EVP_MAX_BLOCK_LENGTH];

    out = (unsigned char *) malloc(sizeof(unsigned char) * (strlen(pwd)+ EVP_MAX_BLOCK_LENGTH));

    // printf("pass: %s\n", pwd);
    // printf("ITERATION: %u\n", ITERATION);

    // if( PKCS5_PBKDF2_HMAC_SHA1(pwd, strlen(pwd), NULL, 0, ITERATION, KEK_KEY_LEN, out) != 0 ) {
    //     printf("out: "); 
    //     for(i=0;i<KEK_KEY_LEN;i++) { 
    //         printf("%02x", out[i]); 
    //     } 
    //     printf("\n");
    // }
    // else {
    //     fprintf(stderr, "PKCS5_PBKDF2_HMAC_SHA1 failed\n");
    // }
    // // getKey(0);

    // free(out);

    // out = getKey(getuid());
    // int i;
    // for(i=0;i<strlen(out);i++) { 
    //     printf("%02x", out[i]); 
    // } 
    // printf("\n");
    // printf("OUT: %s\n", out);    

    printf("Input: %s", pwd);
    getEncrypted(pwd, getuid(), 1, out);
    // if (out != NULL) {
        // int i;
        // for(i=0;i<strlen(out);i++) { 
        //     printf("%02x", out[i]); 
        // } 
        // printf("\n");
    // }
    // printf("Enc: %s\n", out);
    getEncrypted(out, getuid(), 0, out);
    // if (out != NULL)
    printf("Dec: %s\n", out);

    return 0;
}

