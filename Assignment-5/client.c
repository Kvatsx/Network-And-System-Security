 /* 
 * @Author: Kaustav Vats 
 * @Roll-Number: 2016048 
 */
 
#include "helper.h"

int main(int argc, char const *argv[]) {
    
    int raw_sock;
    raw_sock = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
    if (raw_sock < 0) {
        perror("[Error]: Socket\n");
        exit(1);
    }

    struct ifreq ifreq_interface;
    memset(ifreq_interface, 0, sizeof(ifreq_interface));
    strncpy(ifreq_interface.ifr_name, "localhost", IFNAMSIZ);

    if ((ioctl(raw_sock, SIOCGIFINDEX, &ifreq_interface)) < 0) {
        perror("[Error]: ioctl\n");
        exit(1);
    }

    
    

    return 0;
}

