/*
 * @Author: Kaustav Vats 
 * @Roll-Number: 2016048 
 */
// https://www.programiz.com/cpp-programming/strings
// https://www.codesdope.com/cpp-string/
#include "helper.h" 

using namespace std;

pthread_t thread1, thread2;

// void * SendMessage(void * arg) {

//     int client_socket = (int) arg;
//     char Buffer[BUFSIZE] = {0};
//     printf("sendMssg\n");
//     while(1) {
//         sleep(1);
//         fgets(Buffer, BUFSIZE, stdin);
//         // printf("Buf %c\n", Buffer);
//         char * newline = strtok(Buffer, "\n");
//         if (strcmp(newline, "exit") == 0) {
//             // printf("Kya back\n");
//             close(client_socket);
//             pthread_exit(&thread2);
//             return NULL;
//         }
//         // printf("lol");
//         // int i;
//         // for (i=0; i<strlen(newline); i++) {
//         //     printf("%d ", newline[i]);
//         // }
//         // printf("%d\n", strlen(newline));
//         // Buffer[strlen(Buffer)] = "\0";
//         if (send(client_socket, newline, strlen(newline), 0) == -1) {
//             perror("send error\n");
//         }
//     }
// }

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

int main(int argc, char const *argv[]) {
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

    // char ticket[32];
    // cout << "Enter Ticket:\t";
    // cin >> ticket;

    // pthread_create(&thread1, NULL, ReceiveMessage, (void *) fd_socket);
    // pthread_create(&thread2, NULL, SendMessage, (void *) fd_socket);
    cout << "My username: " << strlen(username) << endl;
    if (send(fd_chat, username, strlen(username), 0) == -1) {
        perror("send error\n");
        exit(1);
    }
    sleep(0.5);
    if (send(fd_chat, Tic, strlen(Tic), 0) == -1) {
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

    // while(1) {
    //     printf("RecvMSsg\n");
    //     while(1) {

    //         char Buffer[BUFSIZE] = {0};
    //         int MssgRecvStatus;
    //         if ((MssgRecvStatus = recv(fd_socket, Buffer, BUFSIZE, 0)) <= 0) {
    //             if (MssgRecvStatus == 0) {
    //                 printf("\nServer closed!\n");
    //             }
    //             else {
    //                 perror("recv error!\n");
    //             }
    //             close(fd_socket);
    //             exit(1);
    //         }
            
    //         printf("%s ", Buffer);
    //         if ( strncmp("Wrong Credentials!", Buffer, 19) == 0 ) {
    //             close(fd_socket);
    //             pthread_exit(&thread1);
    //             exit(1);
    //         }
    //     }
    // }

    // // pthread_join(thread1, NULL);
    // pthread_join(thread2, NULL);

    // close(fd_socket);
    return 0;
}
