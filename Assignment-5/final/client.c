 /* 
  * Kaustav Vats
  * 2016048
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "utils.h"

pthread_t thread1;

void * Recv(void * argv) {
    int sock_r;
    struct sockaddr s_addr;
    int s_addr_size = sizeof(s_addr);
    
    int DataSize;

    unsigned char * buf = (unsigned char *) malloc(BUFSIZE2);
    sock_r = socket(AF_INET , SOCK_RAW , IPPROTO_TCP);
    if (sock_r < 0) {
        perror("[Error] socket\n");
        pthread_exit(NULL);
        exit(1);
    }

    while(1) {
        DataSize = recvfrom(sock_r, buf, BUFSIZE2, 0, &s_addr, &s_addr_size);
        if (DataSize < 0) {
            printf("[Error] recvfrom\n");
            pthread_exit(NULL);
            exit(1);
        }
        CheckPacket(buf, DataSize);
    }
    close(sock_r);
    printf("[Done]\n");
    pthread_exit(NULL);
    return 0;
}

int main(int argc, char const *argv[]) {
    
    if (argc != 4) {
        printf("[Error] Usage: ./client -sS\-sF <Destination.IP.Addr> <Source.IP.Addr>\n");
        exit(1);
    }

    // ---------------------------------------------------
    printf("Starting Port Scanner...\n");

    int raw_sock;
    raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (raw_sock < 0) {
        perror("[Error]: Socket\n");
        exit(1);
    }

    char Buffer[BUFSIZE];
    memset(Buffer, 0, BUFSIZE);

    struct iphdr *iph = (struct iphdr *) Buffer;
    struct tcphdr *tcph = (struct tcphdr *) (Buffer + sizeof (struct ip));
    struct pseudo_header psh;

    struct sockaddr_in  socket_addr;
    int socket_addr_len = sizeof(socket_addr);
    socket_addr.sin_family = AF_INET;


    if ( inet_addr(argv[2]) != -1 ) {
        socket_addr.sin_addr.s_addr = inet_addr(argv[2]);
        dest_ip.s_addr = inet_addr(argv[2]);
    }

    printf("Local IP %s \n" , argv[3]);

    set_ip_header(iph, Buffer, argv[3]);
    set_tcp_header(tcph);
    
    int opt = 1;
    if (setsockopt (raw_sock, IPPROTO_IP, IP_HDRINCL, &opt, sizeof (opt)) < 0) {
        perror("[Error] setsockopt\n");
        exit(1);
    }
    printf("Started sniffing...\n");

    if( pthread_create( &thread1, NULL ,  Recv, NULL) < 0) {
        perror("[Error] pthread\n");
        exit(1);
    }
    if (strcmp("-sS", argv[1]) == 0) {
        tcph->fin=0;
        tcph->syn=1;
        tcph->rst=0;
        tcph->psh=0;
        tcph->ack=0;
        tcph->urg=0;
    }
    else {
        tcph->fin=1;
        tcph->syn=0;
        tcph->rst=0;
        tcph->psh=0;
        tcph->ack=0;
        tcph->urg=0;
    }
    int port;
    for (port=1; port < TOTAL_PORTS; port++) {
        tcph->dest = htons(port);
        tcph->check = 0;

        psh.source_address = inet_addr(argv[3]);
        psh.dest_address = socket_addr.sin_addr.s_addr;
        psh.placeholder = 0;
        psh.protocol = IPPROTO_TCP;
        psh.tcp_length = htons( sizeof(struct tcphdr) );

        memcpy(&psh.tcp , tcph , sizeof (struct tcphdr));

        tcph->check = csum( (unsigned short*) &psh , sizeof (struct pseudo_header));
        
        if ( sendto (raw_sock, Buffer , sizeof(struct iphdr) + sizeof(struct tcphdr) , 0 , (struct sockaddr *) &socket_addr, sizeof (socket_addr)) < 0) {
            perror("[Error] send\n");            
            exit(1);
        }
    }
    if (strcmp("-sF", argv[1]) == 0) {
        sleep(7);
        int i;
        for (i=1; i<TOTAL_PORTS; i++) {
            if (r[i] == 0) {
                printf("Port %d open \n" , i);
            } 
        }
    }

    pthread_join(thread1, NULL);

    return 0;
}

