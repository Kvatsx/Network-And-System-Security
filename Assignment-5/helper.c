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
