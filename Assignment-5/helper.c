 /* 
 * @Author: Kaustav Vats 
 * @Roll-Number: 2016048 
 */

#include "helper.h"

void Ethernet_Header(char * input) {
    struct ethhdr * ether = (struct ethhdr *) input;
    printf("-----Ethernet Header-----\n");
    printf("\t| Source Address:\t %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", ether->h_source[0], ether->h_source[1], ether->h_source[2], ether->h_source[3], ether->h_source[4],ether->h_source[5]);
    printf("\t| Destination Address:\t  %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n" ,ether->h_dest[0], ether->h_dest[1], ether->h_dest[2], ether->h_dest[3], ether->h_dest[4], ether->h_dest[5]);
    printf("\t| Protocol: %d\n", ether->h_proto);
}

void IP_Header(char * input) {
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *) (input + sizeof(struct ethhdr));

    struct sockaddr_in source;
    memset(&source, 0, sizeof(source));
    // source.sin_addr.s_addr = ip.
}

int get_local_ip ( char * buffer) {
    int sock = socket ( AF_INET, SOCK_DGRAM, 0);

    const char* kGoogleDnsIp = "8.8.8.8";
    int dns_port = 53;

    struct sockaddr_in serv;

    memset( &serv, 0, sizeof(serv) );
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(kGoogleDnsIp);
    serv.sin_port = htons( dns_port );

    int err = connect( sock , (const struct sockaddr*) &serv , sizeof(serv) );

    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (struct sockaddr*) &name, &namelen);

    const char *p = inet_ntop(AF_INET, &name.sin_addr, buffer, 100);

    close(sock);
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
        *((u_char*) &oddbyte) = *(u_char*) ptr;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;

    return(answer);
}

void process_packet(unsigned char* buffer, int size)
{
    //Get the IP Header part of this packet
    struct iphdr *iph = (struct iphdr*)buffer;
    struct sockaddr_in source,dest;
    unsigned short iphdrlen;
     
    if(iph->protocol == 6)
    {
        struct iphdr *iph = (struct iphdr *)buffer;
        iphdrlen = iph->ihl*4;
     
        struct tcphdr *tcph=(struct tcphdr*)(buffer + iphdrlen);
             
        memset(&source, 0, sizeof(source));
        source.sin_addr.s_addr = iph->saddr;
     
        memset(&dest, 0, sizeof(dest));
        dest.sin_addr.s_addr = iph->daddr;
         
        if(tcph->syn == 1 && tcph->ack == 1 && source.sin_addr.s_addr == dest_ip.s_addr )
        {
            printf("Port %d open \n" , ntohs(tcph->source));
            fflush(stdout);
        }
    }
}