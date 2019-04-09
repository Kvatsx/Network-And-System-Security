/*
 * @Author: Kaustav Vats 
 * @Roll-Number: 2016048 
 */
// https://www.programiz.com/cpp-programming/strings
// https://www.codesdope.com/cpp-string/
#include "helper.h" 

using namespace std;

int main(int argc, char const *argv[]) {
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
