 /* 
 * @Author: Kaustav Vats 
 * @Roll-Number: 2016048 
 */
 
#include "helper.h"

pthread_t thread_id;

struct thread_arg {
    int sock_r;
    struct sockaddr saddr;
    int sock_addr_len;
};

// void * Recv(void * argv) {
//     struct thread_arg * args = (struct thread_arg *) argv;
//     int raw_sock = args->sock_r;
//     struct sockaddr socket_addr  = args->saddr;
//     int socket_addr_len = args->sock_addr_len;

//     unsigned char * Buffer = (unsigned char *) malloc(BUFSIZE * sizeof(unsigned char));
//     memset(Buffer, 0, BUFSIZE);

//     while (1) {
//         if (recvfrom(raw_sock, Buffer, BUFSIZE, 0, &socket_addr, (socklen_t *)&socket_addr_len) < 0) {
//             perror("[Error] Recvfrom\n");
//             pthread_exit(NULL);
//             exit(1);
//         }

//         printf("Output\n");
//         printf("%s\n", Buffer);

//         Ethernet_Header(Buffer);
//         // IP_Header(Buffer);


//     }
//     free(Buffer);
// }

void * Recv(void * argv) {
    int raw_sock;
    struct sockaddr sock_addr;
    int sock_addr_len = sizeof(sock_addr);

    unsigned char * Buffer = (unsigned char *) malloc(BUFSIZE * sizeof(unsigned char));
    memset(Buffer, 0, BUFSIZE);

    raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (raw_sock < 0) {
        perror("[Error] socket\n");
        pthread_exit(NULL);
        exit(1);
    }

    while (1) {
        int dataSize;
        if ((dataSize = recvfrom(raw_sock, Buffer, BUFSIZE, 0, &sock_addr, &sock_addr_len)) < 0) {
            perror("[Error] Recvfrom\n");
            pthread_exit(NULL);
            exit(1);
        }
        process_packet(Buffer, dataSize);
    }
    
}

int main(int argc, char const *argv[]) {
    
    if (argc != 3) {
        printf("[Error] Usage: ./client -sS 127.0.0.1 \n");
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
    struct tcphdr *tcph = (struct tcphdr *) (Buffer + sizeof(struct ip));

    struct sockaddr_in socket_addr;
    int socket_addr_len = sizeof(socket_addr);

    if ( inet_addr(argv[2]) != -1 ) {
        socket_addr.sin_addr.s_addr = inet_addr(argv[2]);
        dest_ip.s_addr = inet_addr(argv[2]);
    }
    socket_addr.sin_family = AF_INET;

    char Source_ip[20];
    get_local_ip(Source_ip);
    printf("Local IP: %s\n", Source_ip);

    // // ------------------------------------------------------

    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct ip) + sizeof(struct tcphdr);
    iph->id = htons (54321); //Id of this packet
    iph->frag_off = htons(16384);
    iph->ttl = 64;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;   
    iph->saddr = inet_addr(Source_ip);
    iph->daddr = socket_addr.sin_addr.s_addr;     
    iph->check = csum ((unsigned short *) Buffer, iph->tot_len >> 1);

    tcph->source = htons ( PORT );
    tcph->dest = htons (80);
    tcph->seq = htonl(1105024978);
    tcph->ack_seq = 0;
    tcph->doff = sizeof(struct tcphdr) / 4;
    tcph->fin=0;
    tcph->syn=1;
    tcph->rst=0;
    tcph->psh=0;
    tcph->ack=0;
    tcph->urg=0;
    tcph->window = htons ( 14600 );
    tcph->check = 0;
    tcph->urg_ptr = 0;

    int one = 1;
    const int *val = &one;

    if (setsockopt(raw_sock, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("[Error] setsockopt\n");
        exit(1);
    }

    printf("Started sniffing...\n");

    // struct ifreq ifreq_interface;
    // memset(ifreq_interface, 0, sizeof(ifreq_interface));
    // strncpy(ifreq_interface.ifr_name, "localhost", IFNAMSIZ);

    // if ((ioctl(raw_sock, SIOCGIFINDEX, &ifreq_interface)) < 0) {
    //     perror("[Error]: ioctl\n");
    //     exit(1);
    // }

    // struct thread_arg arguments;
    // arguments.sock_r = raw_sock;
    // arguments.saddr = socket_addr;
    // arguments.sock_addr_len = socket_addr_len;

    if (pthread_create(&thread_id, NULL, Recv, NULL) ) {
        perror("[Error] pthread\n");
        exit(1);
    }

    struct pseudo_header psh;
    // Syn Packets
    if (strcmp("-sS", argv[1]) == 0) {
        int portn;
        for (portn=1; portn < 1000; portn++) {

            psh.source_address = inet_addr(Source_ip);
            psh.dest_address = socket_addr.sin_addr.s_addr;
            psh.placeholder = 0;
            psh.protocol = IPPROTO_TCP;
            psh.tcp_length = htons( sizeof(struct tcphdr) );
            memcpy(&psh.tcp , tcph , sizeof (struct tcphdr));         
            tcph->dest = htons(portn);
            tcph->check = csum( (unsigned short*) &psh , sizeof (struct pseudo_header));

            if (sendto(raw_sock, Buffer, sizeof(struct iphdr) + sizeof(struct tcphdr), 0, (struct sockaddr *) &socket_addr, sizeof(socket_addr)) < 0) {
                perror("[Error] send\n");
                exit(1);
            }
        }

    }
    else {
        
    }
    pthread_join(thread_id, NULL);

    return 0;
}

