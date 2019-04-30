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
int global_ret;

void * Recv(void * argv) {
    int lol = (int) argv;
    int sock_r;
    struct sockaddr s_addr;
    int s_addr_size = sizeof(s_addr);
    
    int DataSize;
    int ret = 0;

    unsigned char * buf = (unsigned char *) malloc(BUFSIZE2);
    sock_r = socket(AF_INET , SOCK_RAW , IPPROTO_ICMP);
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
        // printf("Packet aya\n");
        ret = CheckUdpPacket(buf, DataSize);
        if (ret == 0)
            global_ret = 0;
    }
    close(sock_r);
    // printf("[Done]\n");
    pthread_exit(NULL);
    return 0;
}

int main(int argc, char const *argv[]) {
    // printf("%d\n", argc);
    if (argc != 4) {
        printf("[Error] Usage: ./nmap_udp -sU <Destination.IP.Addr> <Source.IP.Addr>\n");
        exit(1);
    }
    global_ret = 1;
    // ---------------------------------------------------
    printf("Starting Port Scanner...\n");

    int raw_sock;
    raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (raw_sock < 0) {
        perror("[Error]: Socket\n");
        exit(1);
    }

    char Buffer[BUFSIZE];
    memset(Buffer, 0, BUFSIZE);

    struct iphdr *iph = (struct iphdr *) Buffer;
    struct udphdr *udph = (struct udphdr *) (Buffer + sizeof (struct ip));
    struct udp_header psh;

    struct sockaddr_in  socket_addr;
    int socket_addr_len = sizeof(socket_addr);
    socket_addr.sin_family = AF_INET;


    if ( inet_addr(argv[2]) != -1 ) {
        socket_addr.sin_addr.s_addr = inet_addr(argv[2]);
        dest_ip.s_addr = inet_addr(argv[2]);
    }

    printf("Local IP %s \n" , argv[3]);

    set_udp_ip_header(iph, Buffer, argv[3]);
    set_udp_header(udph);
    
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

    int port;
    for (port=1; port < 10; port++) {
        udph->dest = htons(port);

        psh.source_address = inet_addr(argv[3]);
        psh.dest_address = socket_addr.sin_addr.s_addr;
        psh.placeholder = 0;
        psh.protocol = IPPROTO_UDP;
        psh.udp_length = htons( sizeof(struct udphdr) );

        memcpy(&psh.udp , udph , sizeof (struct udphdr));

        udph->check = csum( (unsigned short*) &psh , sizeof (struct udp_header));
        
        if ( sendto (raw_sock, Buffer , sizeof(struct iphdr) + sizeof(struct udphdr) , 0 , (struct sockaddr *) &socket_addr, sizeof (socket_addr)) < 0) {
            perror("[Error] send\n");            
            exit(1);
        }
        // printf("Before %d\n", global_ret);
        sleep(5);
        // printf("After %d\n", global_ret);
        if (global_ret != 0) {
            printf("Port %d open|filtered\n", port);
        }
        global_ret = 1;

    }

    pthread_join(thread1, NULL);

    return 0;
}

