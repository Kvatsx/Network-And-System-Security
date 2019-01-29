/*
 * @Author: Kaustav Vats 
 * @Roll-Number: 2016048 
 * @Date: 2019-01-08 21:04:50 
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <pthread.h>
#include <dirent.h>
#include <math.h>

#define PORT 5555
#define BUFSIZE 1024
#define CONNECTIONS 6

char root[BUFSIZE];

char usernames[5][2];

struct Arguments {
    int client_socket;
    int server_socket;
    char user[2];
};

void pwd() {
	char currentwd[1024];
	getcwd(currentwd, sizeof(currentwd));
	printf("pwd: %s\n", currentwd );
}