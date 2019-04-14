/*
 * @Author: Kaustav Vats 
 * @Roll-Number: 2016048 
 */
// https://www.programiz.com/cpp-programming/strings
// https://www.codesdope.com/cpp-string/
#include "helper.h" 

using namespace std;

pthread_t thread1, thread2;
string user;

void * SendMessage(void * argv) {
    int fd = *((int*) argv);
    cout << "fd " << fd << endl;
    while(1) {
        // char input[BUFSIZE];
        // memset(input, '\0', sizeof(input));
        // cin >> input;
        char *input = NULL;
        size_t size;
        getline(&input, &size, stdin);
        input[strlen(input)-1] = '\0';
        cout << "Input: " << input << endl;
        if (strlen(input) == 0) {
            continue;
        }
        // cout << "L: " << strlen(input) << endl;
        // unsigned char *out;
        // out = (unsigned char *) malloc(sizeof(unsigned char) * BUFSIZE);
        // do_enc((const unsigned char *)input, user, 1, out);

        cout << "out: " << input << endl;
        sleep(1);
        if (send(fd, input, strlen((const char *)input), 0) == -1) {
            perror("send error\n");
            exit(1);
        }
    }
}

int TalkToKDC(const char * argv, char * ticket) {
    int fd_socket;
    struct sockaddr_in sock_address;

    if ((fd_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	    perror("Socket error!\n");
	    exit(1);
    }

    memset(&sock_address, '0', sizeof(sock_address)); 
    sock_address.sin_family = AF_INET;
    sock_address.sin_port = htons(KDC_PORT);

    cout << "connecting:\t" << argv << endl;
    if ( inet_pton(AF_INET, argv, &sock_address.sin_addr) <= 0 ) {
        perror("Invalid Address!\n");
        exit(1);
    }

    memset(sock_address.sin_zero, '\0', sizeof(sock_address.sin_zero));

    if ( connect(fd_socket, (struct sockaddr *)&sock_address, sizeof(struct sockaddr_in)) < 0 ) {
        perror("Connection Fail\n");
        exit(1);
    }

    cout << "Enter username: ";
    char username[100];
    char password[100];
    cin >> username;

    cout << "Enter Password: ";
    cin >> password;

    // cout << "Entered username and password\t" << username << "\t" << password << endl;

    if (send(fd_socket, username, strlen(username), 0) == -1) {
        perror("send error\n");
        return -1;
    }
    sleep(0.5);
    if (send(fd_socket, password, strlen(password), 0) == -1) {
        perror("send error\n");
        return -1;
    }

    char Buffer[BUFSIZE] = {0};
    int MssgRecvStatus;
    if ((MssgRecvStatus = recv(fd_socket, Buffer, BUFSIZE, 0)) <= 0) {
        if (MssgRecvStatus == 0) {
            printf("\nServer closed!\n");
            return -1;
        }
        else {
            perror("recv error!\n");
            return -1;
        }
        exit(1);
    }
    cout << Buffer << "\n" << endl;    
    if ( strncmp("No user exist with these credentials!", Buffer, 38) == 0 ) {
        close(fd_socket);
        exit(1);
    }
    memset(Buffer, '\0', sizeof(Buffer));
    if (recv(fd_socket, Buffer, sizeof(Buffer), 0) <= 0) {
        cout << "Client Closed or Recv Error" << endl;
    }
    // cout << "Ticket:\t" << Buffer << endl;
    strncpy(ticket, Buffer, 32);
    close(fd_socket);
    return 1;
}
// void ReadKeyAndIv() {
//     struct passwd * pwd = getpwuid(getuid());
    
//     FILE * fd;
//     char * line = NULL;
//     size_t len = 0;
//     ssize_t read;

//     fd = fopen("/etc/shadow", "r");
//     if (fd == NULL) {
//         printf("File could not be opened.\n");
//         perror("open");
//         return;
//     }

//     KEY = (unsigned char *) malloc(sizeof(unsigned char) * 32);   
//     IV = (unsigned char *) malloc(sizeof(unsigned char) * 16); 

//     while ((read = getline(&line, &len, fd)) != -1) {

//         char * token = strtok(line, ":");
//         if (strcmp(token, pwd->pw_name) == 0) {
//             token = strtok(NULL, ":");
//             int i;

//             if( PKCS5_PBKDF2_HMAC_SHA1(token, strlen(token), NULL, 0, 100, 32, KEY) == 0 ) {
//                 perror("PBKDF");
//             }
//             if( PKCS5_PBKDF2_HMAC_SHA1(token, strlen(token), NULL, 0, 50, 16, IV) == 0 ) {
//                 perror("PBKDF");
//             }
//             // close(fd);
//             // return out;
//         }
//     }
//     fclose(fd);
// }

int main(int argc, char const *argv[]) {
    // Read Key and Iv
    // ReadKeyAndIv();
    cout << "Key collected!" << endl;
    // Talking to KDC
    char * Tic;
    Tic = (char *) malloc(sizeof(char) * 32);
    if (TalkToKDC(argv[1], Tic) == -1) {
        cout << "Error: Connecting to KDC" << endl;
        exit(1);
    }
    cout << "Returned Ticket " << Tic << endl;

    // Connect to server on Chat Port
    int fd_chat;
    struct sockaddr_in sock_chat;

    if ((fd_chat = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	    perror("Socket error!\n");
	    exit(1);
    }

    memset(&sock_chat, '0', sizeof(sock_chat)); 
    sock_chat.sin_family = AF_INET;
    sock_chat.sin_port = htons(CHAT_PORT);

    if ( inet_pton(AF_INET, argv[1], &sock_chat.sin_addr) <= 0 ) {
        perror("Invalid Address!\n");
        exit(1);
    }

    memset(sock_chat.sin_zero, '\0', sizeof(sock_chat.sin_zero));

    if ( connect(fd_chat, (struct sockaddr *)&sock_chat, sizeof(struct sockaddr_in)) < 0 ) {
        perror("Connection Fail\n");
        exit(1);
    }

    cout << "Enter username: ";
    char username[100];
    cin >> username;
    user = username;

    // char ticket[32];
    // cout << "Enter Ticket:\t";
    // cin >> ticket;

    // pthread_create(&thread2, NULL, SendMessage, (void *) fd_socket);
    cout << "My username: " << strlen(username) << endl;
    if (send(fd_chat, username, strlen(username), 0) == -1) {
        perror("send error\n");
        exit(1);
    }
    sleep(1);
    if (send(fd_chat, Tic, 32, 0) == -1) {
        perror("send error\n");
        exit(1);
    }

    char Buffer[BUFSIZE];
    if (recv(fd_chat, Buffer, sizeof(Buffer), 0) <= 0) {
        cout << "Client Closed or Recv Error" << endl;
    }

    cout << Buffer << "\n" << endl;    
    if ( strncmp("Ticket not valid!", Buffer, 18) == 0 ) {
        close(fd_chat);
        exit(1);
    }
    memset(Buffer, '\0', sizeof(Buffer));
    if (recv(fd_chat, Buffer, sizeof(Buffer), 0) <= 0) {
        cout << "Client Closed or Recv Error" << endl;
    }

    cout << "Last:" << Buffer << endl;

    if (pthread_create(&thread1, NULL, SendMessage, &fd_chat)) {
        cout << "Unable to create a thread" << endl;
    }
     
    while(1) {

        char Buffer[BUFSIZE] = {0};
        int MssgRecvStatus;
        if ((MssgRecvStatus = recv(fd_chat, Buffer, BUFSIZE, 0)) <= 0) {
            if (MssgRecvStatus == 0) {
                printf("\nServer closed!\n");
            }
            else {
                perror("recv error!\n");
            }
            close(fd_chat);
            exit(1);
        }
        cout << Buffer << endl;
    }

    // // pthread_join(thread1, NULL);
    pthread_join(thread1, NULL);

    close(fd_chat);
    return 0;
}
