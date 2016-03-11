#ifndef tcp_client_h
#define tcp_client_h

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int tcp_connect(struct in_addr ip,int port,char keyfile[], char * a_name, char * a_surname);
int authentication_client(struct sockaddr_in addr, char *keyfile, int fd);
void write_tcp(char * ptr, int fd);
char * read_tcp(int fd);

#endif /* udp_socket_client_h */
