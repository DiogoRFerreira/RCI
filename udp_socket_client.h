#ifndef udp_socket_client_h
#define udp_socket_client_h

#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

element * udp_socket_to_snp(struct in_addr ip,int port,char** message);

void udp_socket(struct in_addr ip,int port,char** message);

element * udp_socket_sa(struct in_addr ip_sa,int port, char surname[20], char name[20]);

element * udp_socket_server(element * ptr_to_first, int * num_elements_ptr, int fd, struct sockaddr_in addr, struct in_addr ip_sa);

#endif /* udp_socket_client_h */
