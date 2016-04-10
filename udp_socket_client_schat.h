#ifndef udp_socket_client_schat_h
#define udp_socket_client_schat_h


#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "list.h"

element * udp_socket(struct in_addr ip,int port,char** message, int action);
element * strtoelem(char buffer[]);


#endif
