# makefile

snp.o: snp.c udp_socket_client.c gethostsname.c list.c
	gcc -g -Wall udp_socket_client.c gethostsname.c list.c snp.c  -o snp

scha.ot: schat.c udp_socket_client_schat.c tcp_client.c
	gcc -g -Wall snp.c udp_socket_client.c gethostsname.c list.c -o schat
