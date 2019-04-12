#ifndef HELPER_H
#define HELPER_H

#include <unistd.h> 
#include <iostream>
#include <sys/socket.h> 
#include <cstring>
#include <stdlib.h> 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/time.h>
#include <cstdlib>
#include <pthread.h>   
#include <iterator> 
#include <map> 
#include <openssl/evp.h>
#include <time.h>
#include <list> 
#include <iterator> 

#define KDC_PORT 5555
#define CHAT_PORT 6666
#define BUFSIZE 5024
#define CONNECTIONS 6

// Server functions
void * KDC_Server(void * argv);
void * ConnectionHandler(void * argv);

// Client functions
void * SendMessage(void * argv);

// Helper.cpp
// void String2Char(string input, char * output);

#endif