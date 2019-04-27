 /* 
 * @Author: Kaustav Vats 
 * @Roll-Number: 2016048 
 */

#include "helper.h"



int main(int argc, char const *argv[]) {
    
    int raw_sock;
    raw_sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_sock < 0) {
        perror("[Error]: Socket\n");
        exit(1);
    }

    unsigned char * Buffer = (unsigned char *) malloc(BUFSIZE * (unsigned char));
    memset(Buffer, 0, BUFSIZE);

    struct sockaddr socket_addr;
    int socket_addr_len = sizeof(socket_addr);

    // Receiving Raw Packet
    int outsize;
    if ((outsize = recvfrom(raw_sock, Buffer, BUFSIZE, 0, &socket_addr, (socklen_t *) socket_addr_len)) < 0) {
        perror("[Error]: Recvfrom\n");
        exit(1);
    }
    Ethernet_Header(Buffer);

    

    return 0;
}
