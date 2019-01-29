/*
 * @Author: Kaustav Vats 
 * @Roll-Number: 2016048 
 * @Date: 2019-01-12 19:00:54 
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>

#define PORT 5555
#define BUFSIZE 1024

pthread_t thread1, thread2;

void * SendMessage(void * arg) {

    int client_socket = (int) arg;
    char Buffer[BUFSIZE] = {0};
    printf("sendMssg\n");
    while(1) {
        sleep(1);
        fgets(Buffer, BUFSIZE, stdin);
        if (strncmp(Buffer, "exit", 4) == 0) {
            close(client_socket);
            pthread_exit(&thread2);
            return NULL;
        }
        char * newline = strtok(Buffer, "\n");
        // int i;
        // for (i=0; i<strlen(newline); i++) {
        //     printf("%d ", newline[i]);
        // }
        // printf("%d\n", strlen(newline));
        // Buffer[strlen(Buffer)] = "\0";
        if (send(client_socket, newline, strlen(newline), 0) == -1) {
            perror("send error\n");
        }
    }
}

// void * ReceiveMessage(void * arg) {

//     int socket = (int) arg;
//     printf("RecvMSsg\n");
//     while(1) {

//         char Buffer[BUFSIZE] = {0};
//         int MssgRecvStatus;
//         if ((MssgRecvStatus = recv(socket, Buffer, BUFSIZE, 0)) <= 0) {
//             if (MssgRecvStatus == 0) {
//                 printf("\nServer closed!\n");
//             }
//             else {
//                 perror("recv error!\n");
//             }
//             close(socket);
//             pthread_exit(&thread1);
//             return NULL;
//         }
        
//         printf("%s", Buffer);
//         if ( strncmp("Wrong Credentials!", Buffer, 19) == 0 ) {
//             close(socket);
//             pthread_exit(&thread1);
//             return NULL;
//         }
//     }
//     return NULL;
// }

int main(int argc, char const *argv[])
{
    int fd_socket;
    struct sockaddr_in sock_address;

    if ((fd_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	    perror("Socket error!\n");
	    exit(1);
    }

    memset(&sock_address, '0', sizeof(sock_address)); 
    sock_address.sin_family = AF_INET;
    sock_address.sin_port = htons(PORT);

    if ( inet_pton(AF_INET, argv[1], &sock_address.sin_addr) <= 0 ) {
        perror("Invalid Address!\n");
        exit(1);
    }

    memset(sock_address.sin_zero, '\0', sizeof(sock_address.sin_zero));

    if ( connect(fd_socket, (struct sockaddr *)&sock_address, sizeof(struct sockaddr_in)) < 0 ) {
        perror("Connection Fail\n");
        exit(1);
    }

    // pthread_create(&thread1, NULL, ReceiveMessage, (void *) fd_socket);
    pthread_create(&thread2, NULL, SendMessage, (void *) fd_socket);
    
    while(1) {
        printf("RecvMSsg\n");
        while(1) {

            char Buffer[BUFSIZE] = {0};
            int MssgRecvStatus;
            if ((MssgRecvStatus = recv(fd_socket, Buffer, BUFSIZE, 0)) <= 0) {
                if (MssgRecvStatus == 0) {
                    printf("\nServer closed!\n");
                }
                else {
                    perror("recv error!\n");
                }
                close(fd_socket);
                exit(1);
            }
            
            printf("%s ", Buffer);
            if ( strncmp("Wrong Credentials!", Buffer, 19) == 0 ) {
                close(fd_socket);
                pthread_exit(&thread1);
                exit(1);
            }
        }
    }

    // pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    close(fd_socket);
    return 0;
}

