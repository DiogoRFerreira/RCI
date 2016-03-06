#ifndef tcp_client_h
#define tcp_client_h

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void tcp_connect(struct in_addr ip,int port);

#endif /* udp_socket_client_h */
