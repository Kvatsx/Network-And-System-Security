#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h> 
// #include <netinet/icmp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "utils.h"
// Ref-> csum function https://www.cnblogs.com/rollenholt/articles/2590959.html

void set_ip_header(struct iphdr * iph, char * buf, char * ipaddr) {
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct ip) + sizeof (struct tcphdr);
    iph->id = htons (54321);
    iph->frag_off = htons(16384);
    iph->ttl = 64;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;   
    iph->saddr = inet_addr ( ipaddr );  
    iph->daddr = dest_ip.s_addr;
     
    iph->check = csum ((unsigned short *) buf, iph->tot_len >> 1);
}

void set_tcp_header(struct tcphdr * tcph) {
    tcph->source = htons ( PORT );
    tcph->dest = htons (80);
    tcph->seq = htonl(1105024978);
    tcph->ack_seq = 0;
    tcph->doff = sizeof(struct tcphdr) / 4;
    tcph->window = htons ( 14600 );  
    tcph->check = 0;
    tcph->urg_ptr = 0;
    
}

void set_udp_ip_header(struct iphdr * iph, char * buf, char * ipaddr) {
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct ip) + sizeof (struct udphdr);
    iph->id = htons (54321);
    iph->frag_off = htons(16384);
    iph->ttl = 64;
    iph->protocol = IPPROTO_UDP;
    iph->check = 0;   
    iph->saddr = inet_addr ( ipaddr );  
    iph->daddr = dest_ip.s_addr;
     
    iph->check = csum ((unsigned short *) buf, iph->tot_len >> 1);
}


void set_udp_header(struct udphdr * udph) {
    udph->source = htons(PORT);
    udph->check = 0;
    udph->len = htons(sizeof(struct udphdr));
}

void CheckPacket(unsigned char * buf, int DataSize) {
    struct iphdr * iph = (struct iphdr *) buf;
    unsigned short iphdrlen;

    if(iph->protocol == 6)
    {   
        struct iphdr *iph = (struct iphdr *)buf;
        iphdrlen = iph->ihl*4;
     
        struct tcphdr *tcph=(struct tcphdr*)(buf + iphdrlen);

        if(tcph->syn == 1 && tcph->ack == 1 && iph->saddr == dest_ip.s_addr )
        {
            printf("Port %d open \n" , ntohs(tcph->source));
        }
        else if (tcph->rst == 1 && tcph->ack == 1 && iph->saddr == dest_ip.s_addr) {
            r[ntohs(tcph->source)] = 1;
        }
    }
}


void CheckUdpPacket(unsigned char * buf, int DataSize) {
    struct iphdr * iph = (struct iphdr *) buf;
    unsigned short iphdrlen;

    if(iph->protocol == 1)
    {
        // printf("Port: %d\n", ntohs(udph->source));
        struct iphdr *iph = (struct iphdr *)buf;
        iphdrlen = iph->ihl*4;
     
        struct icmphdr *icmph=(struct icmphdr*)(buf + iphdrlen);
        
        if(iph->saddr == dest_ip.s_addr )
        {
            
            printf("Port %d \n" , icmph->code);
        }
    }
}

void printDetails(struct tcphdr* tcph, struct iphdr* iph) {
    printf("Port: %d, ips: %d, ipd: %d, syn: %d, ack: %d\n", ntohs(tcph->source), iph->saddr, iph->daddr, tcph->syn, tcph->ack);
}   

unsigned short csum(unsigned short *ptr,int nbytes)
{
    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }
 
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
     
    return(answer);
}
