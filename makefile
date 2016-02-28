# makefile

snp: snp.c udp_socket_client.c gethostsname.c list.c
	gcc -g -Wall udp_socket_client.c gethostsname.c list.c snp.c -o snp
