#ifndef HELPER_H
#define HELPER_H

#define BUFSIZE 4096
#define BUFSIZE2 65536
#define PORT 5555
#define TOTAL_PORTS 1000

int r[TOTAL_PORTS+1];

struct in_addr dest_ip;
struct pseudo_header
{
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short tcp_length;
     
    struct tcphdr tcp;
};

struct udp_header
{
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short udp_length;
     
    struct udphdr udp;
};

void set_ip_header(struct iphdr * iph, char * buf, char * ipaddr);
void set_udp_ip_header(struct iphdr * iph, char * buf, char * ipaddr);

void set_tcp_header(struct tcphdr * tcph);
void set_udp_header(struct udphdr * udph);
void CheckPacket(unsigned char * buf, int DataSize);
int CheckUdpPacket(unsigned char * buf, int DataSize);

unsigned short csum(unsigned short *ptr,int nbytes);
void printDetails(struct tcphdr* tcph, struct iphdr* iph);


#endif
