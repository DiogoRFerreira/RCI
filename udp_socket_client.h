#ifndef udp_socket_client_h
#define udp_socket_client_h

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void udp_socket(struct in_addr ip,int port,char** message);

void udp_socket_server();

#endif /* udp_socket_client_h */
