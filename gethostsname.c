#include "gethostsname.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>//gethostname
#include <string.h>
#include <errno.h>
#include <netdb.h>//gethostbyname
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>

extern int errno;

struct in_addr getaddressbyname(const char* name){
    
    struct in_addr *a;
    struct hostent *h;
    
    if((h=gethostbyname(name))==NULL){//error
        exit(1);
    }
    
    printf("official host name: %s\n",h->h_name);
    a=(struct in_addr*)h->h_addr_list[0];
    printf("internet address: %s (%08lX)\n",inet_ntoa(*a),(long unsigned int)ntohl(a->s_addr));
    
    return *a;
}
/*
//Passar por refência para alterar valor funcntion(&name)    function (char * name)
void get_host_name(struct in_addr *compare){
    
    struct in_addr *a;
    char *hostname[128];

    if(gethostname(hostname,128)==-1){
        printf("error: %s\n",strerror(errno));
    }else{printf("host name: %s\n",*hostname);}
    
    *a=get_address_by_name();
    
}
*/







