/*
 * @Author: Kaustav Vats 
 * @Roll-Number: 2016048 
 */

#include "helper.h"

using namespace std;

// unsigned char * do_crypt(char * input, uid_t uid, int encdec, unsigned char * out) {
//     unsigned char *key;
//     unsigned char *iv;

//     key = (unsigned char *) malloc(sizeof(unsigned char) * 32);   
//     iv = (unsigned char *) malloc(sizeof(unsigned char) * 16); 

//     // iv = getKey(uid);
//     // unsigned char iv[] = "1234567887654321";
//     // printf("Key %s\n%d\n", key, strlen(key));
//     // printf("Iv %s\n%d\n", iv, strlen(iv));

//     unsigned char * output[strlen(input) + EVP_MAX_BLOCK_LENGTH];
//     // memset(output, '\0', strlen(output));
//     int input_len, output_len;

//     EVP_CIPHER_CTX *ctx;
//     ctx = EVP_CIPHER_CTX_new();

//     EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), NULL, NULL, NULL, encdec);
//     OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 32);
//     OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);
//     EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, encdec);

//     if(!EVP_CipherUpdate(ctx, output, &output_len, input, input_len)) {
//         perror("Here");
//         EVP_CIPHER_CTX_free(ctx);
//         return NULL;
//     }

//     strcpy(out, output);

//     // if(!EVP_CipherFinal_ex(ctx, output, &output_len)) {
//     //     perror("where");
//     //     EVP_CIPHER_CTX_free(ctx);
//     //     return NULL;
//     // }

//     EVP_CIPHER_CTX_free(ctx);

//     return output;

// }
// void String2Char(string input, char * output) {

// }
