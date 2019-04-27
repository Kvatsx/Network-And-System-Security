#ifndef HELPER_H
#define HELPER_H
 
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define BUFSIZE 4096
#define PORT 5555

struct pseudo_header {
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short tcp_length;     
    struct tcphdr tcp;
};

struct in_addr dest_ip;

void Ethernet_Header(char * input);
void IP_Header(char * input);

void process_packet(unsigned char* buffer, int size);
unsigned short csum(unsigned short *ptr,int nbytes);
int get_local_ip ( char * buffer);


#endif
