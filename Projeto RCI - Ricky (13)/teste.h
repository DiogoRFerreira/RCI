#ifndef teste
#define teste
#include "struct_no.h"

char *connections(char * connection, struct info_no * no);
void connecting_new(char * connection, struct info_no * no);
char * tcp_searching(char *searching, struct info_no *no, int flag_search);
void connecting_new_to_arranque(char * connection, struct info_no * no);
char * getip1(char * cmd);
int search_check(int l, int k, int predi);
int dist(int k, int l);
#endif

