#ifndef udp_socket_client_h
#define udp_socket_client_h

#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void udp_socket(struct in_addr ip,int port,char** message);

element * udp_socket_server(element * ptr_to_first, int * num_elements_ptr, int fd, struct sockaddr_in addr, socklen_t addrlen);

#endif /* udp_socket_client_h */
