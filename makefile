# makefile

all: snp schat

snp: snp.c udp_socket_client.c gethostsname.c list.c
	gcc -g -Wall udp_socket_client.c gethostsname.c list.c snp.c  -o snp

schat: schat.c udp_socket_client_schat.c tcp_client.c list.c
	gcc -g -Wall udp_socket_client_schat.c list.c tcp_client.c schat.c -o schat
