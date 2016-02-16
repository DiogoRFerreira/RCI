#ifndef teste1
#define teste1
#include "struct_no.h"

void select_tcp(int flag, int desc, int flag_predi, int flag_succi, int tcp_port, char * ip2, int tcp_port1, struct info_no * no, char * udp_ip, int udp_port);
int unreg(char * ip, int boot_port, int tcp_port, struct info_no * no, int succi_index, int predi_index);
#endif

