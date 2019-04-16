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

// void sslEnc() {
//  SSL_load_error_strings();
//   OpenSSL_add_ssl_algorithms();
//   meth = TLS_server_method();
//   ctx = SSL_CTX_new (meth);
//   if (!ctx) {
//     ERR_print_errors_fp(stderr);
//     exit(2);
//   }
  
//   if (SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM) <= 0) {
//     ERR_print_errors_fp(stderr);
//     exit(3);
//   }
//   if (SSL_CTX_use_PrivateKey_file(ctx, KEYF, SSL_FILETYPE_PEM) <= 0) {
//     ERR_print_errors_fp(stderr);
//     exit(4);
//   }

//   if (!SSL_CTX_check_private_key(ctx)) {
//     fprintf(stderr,"Private key does not match the certificate public key\n");
//     exit(5);
//   }

//   /* ----------------------------------------------- */
//   /* Prepare TCP socket for receiving connections */

//   listen_sd = socket (AF_INET, SOCK_STREAM, 0);   CHK_ERR(listen_sd, "socket");
  
//   memset(&sa_serv, 0, sizeof(sa_serv));
//   sa_serv.sin_family      = AF_INET;
//   sa_serv.sin_addr.s_addr = INADDR_ANY;
//   sa_serv.sin_port        = htons (1111);          /* Server Port number */
  
//   err = bind(listen_sd, (struct sockaddr*) &sa_serv,
// 	     sizeof (sa_serv));                   CHK_ERR(err, "bind");
	     
//   /* Receive a TCP connection. */
	     
//   err = listen (listen_sd, 5);                    CHK_ERR(err, "listen");
  
//   client_len = sizeof(sa_cli);
//   sd = accept (listen_sd, (struct sockaddr*) &sa_cli, &client_len);
//   CHK_ERR(sd, "accept");
//   close (listen_sd);

//   printf ("Connection from %lx, port %x\n",
// 	  sa_cli.sin_addr.s_addr, sa_cli.sin_port);
  
//   /* ----------------------------------------------- */
//   /* TCP connection is ready. Do server side SSL. */

//   ssl = SSL_new (ctx);                           CHK_NULL(ssl);
//   SSL_set_fd (ssl, sd);
//   err = SSL_accept (ssl);                        CHK_SSL(err);
  
//   /* Get the cipher - opt */
  
//   printf ("SSL connection using %s\n", SSL_get_cipher (ssl));
  
//   /* Get client's certificate (note: beware of dynamic allocation) - opt */

//   client_cert = SSL_get_peer_certificate (ssl);
//   if (client_cert != NULL) {
//     printf ("Client certificate:\n");
    
//     str = X509_NAME_oneline (X509_get_subject_name (client_cert), 0, 0);
//     CHK_NULL(str);
//     printf ("\t subject: %s\n", str);
//     OPENSSL_free (str);
    
//     str = X509_NAME_oneline (X509_get_issuer_name  (client_cert), 0, 0);
//     CHK_NULL(str);
//     printf ("\t issuer: %s\n", str);
//     OPENSSL_free (str);
    
//     /* We could do all sorts of certificate verification stuff here before
//        deallocating the certificate. */
    
//     X509_free (client_cert);
//   } else
//     printf ("Client does not have certificate.\n");

//   /* DATA EXCHANGE - Receive message and send reply. */

//   err = SSL_read (ssl, buf, sizeof(buf) - 1);                   CHK_SSL(err);
//   buf[err] = '\0';
//   printf ("Got %d chars:'%s'\n", err, buf);
  
//   err = SSL_write (ssl, "I hear you.", strlen("I hear you."));  CHK_SSL(err);

//   /* Clean up. */

//   close (sd);
//   SSL_free (ssl);
//   SSL_CTX_free (ctx);
// }

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

    int outlen, val_lav;

    EVP_CIPHER_CTX *ctx;

    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    EVP_EncryptUpdate(ctx, out, &outlen, input, strlen((const char *) input));
    val_lav = outlen;
    EVP_EncryptFinal_ex(ctx, out + outlen, &outlen);
    
    val_lav += outlen;
    EVP_CIPHER_CTX_free(ctx);
    // out[outlen] = "\0";
    // printf("OUTPUT: %s\n", out);
    return val_lav;
}

int do_dec(const unsigned char * input, string username, int encdec, unsigned char * out) {
    unsigned char *key;
    unsigned char *iv;

    key = (unsigned char *) malloc(sizeof(unsigned char) * 32);   
    iv = (unsigned char *) malloc(sizeof(unsigned char) * 16); 

    getKeyIv(username, key, iv);

    int outlen, tmplen, val_lav;

    EVP_CIPHER_CTX *ctx;

    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    EVP_DecryptUpdate(ctx, out, &outlen, input, strlen((const char *)input));
    val_lav = outlen;
    EVP_DecryptFinal_ex(ctx,(unsigned char *) (out + outlen), &outlen);

    val_lav += outlen;
    // cout << "Outlen: " << outlen << endl;
    // printf("OUTPUT: %s\n", out);

    // out[outlen] = reinterpret_cast<const unsigned char *>( "\0") ;
    EVP_CIPHER_CTX_free(ctx);
    return val_lav;
}
void getKeyIv(string username, unsigned char * key, unsigned char * iv) {
    username = "fakeroot";
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
            break;
        }
    }
    fclose(fd);
}

void init_diffi(std::string user, string &gobar) {
    // creating nonce 
    int number = rand() % 9000 + 1000;
    gobar += "P: ";

    // cout << "P: " << number << endl;
    string num = to_string(number);
    gobar += num;
    gobar += "\n";
    unsigned char * prev = (unsigned char *) malloc(32 * sizeof(unsigned char));
    PKCS5_PBKDF2_HMAC_SHA1(num.c_str(), strlen(num.c_str()), NULL, 0, 100, 32, prev);
    sleep(0.7);
    char out[33];
    strncpy(out, (const char *) prev, 32);
    out[32] = '\0';
    // cout << "gPower: " << out << endl;
    // string newout(out);
    gobar += "gPower: ";
    gobar += out;
    gobar += "\n";
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

void encdec(const unsigned char * input, string username, int encdec, unsigned char * out) {
    unsigned char *key;
    unsigned char *iv;

    key = (unsigned char *) malloc(sizeof(unsigned char) * 32);   
    iv = (unsigned char *) malloc(sizeof(unsigned char) * 16); 
    
    getKeyIv(username, key, iv);

    AES_KEY EncDec;
    AES_set_decrypt_key((const unsigned char *) KEY, 128, &EncDec);
    if (encdec == 0) {
        // Decrypt
        AES_decrypt((const unsigned char *) input, out, (const AES_KEY *) &EncDec);
    }
    else {
        AES_encrypt((const unsigned char *) input, out, (const AES_KEY *) &EncDec);
    }
}



int getch() {
    int ch;
    struct termios t_old, t_new;

    tcgetattr(STDIN_FILENO, &t_old);
    t_new = t_old;
    t_new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);

    return ch;
}

void getPass(char *prompt, int show_asterisk, char * password)
{
    int BACKSPACE = 127;
    int RETURN = 10;

    int count = 0;
    int ch = 0;

    printf("%s: ", prompt);

    while ((ch = getch()) != RETURN)
    {

        if (ch == BACKSPACE)
        {
            if (count != 0) {
                if (show_asterisk != 0) {
                    printf("\b \b");
                }
                count--;
            }
        }
        else
        {
            char temp = ch;
            password[count] = temp;
            count++;
            if (show_asterisk != 0) {
                printf("*");
            }
        }
    }
    password[count] = '\0';
    printf("\n");
}

int Encrypted_send(char * input, string user, int fd) {
    unsigned char *prevprev;
    unsigned char *verpverp;

    prevprev = (unsigned char *) malloc(sizeof(unsigned char) * 32);   
    verpverp = (unsigned char *) malloc(sizeof(unsigned char) * 16); 

    getKeyIv(user, prevprev, verpverp);

    cout << "Final Key: " << prevprev << endl;
    return 1;
}

int Decrypted_recv(char * out, string user, int fd) {
    char input[BUFSIZE];
    if ( recv(fd, input, BUFSIZE, 0) <= 0) {
        cout << "Client Closed or Recv Error" << endl;
        return -1;
    }

    int ret = do_dec((const unsigned char *) input, user, 0, (unsigned char *) out);
    out[ret] = '\0';
}


int checkUsername(char * username) {
    struct passwd * pwd = getpwuid(getuid());
    // cout << "Expected user: " << pwd->pw_name << endl; 

    if (strcmp(pwd->pw_name, username) != 0) {
        cout << "Thats not your real name!" << endl;
        return -1;
    }
    return 1;
}