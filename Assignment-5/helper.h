#ifndef HELPER_H
#define HELPER_H
 
#include <unistd.h>
#include <stdio.h>
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
#include <string.h>
#include <arpa/inet.h>

#define BUFSIZE 65536

void Ethernet_Header(char * input);

#endif
