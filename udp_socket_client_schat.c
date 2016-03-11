#include "udp_socket_client_schat.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

element * udp_socket(struct in_addr ip,int port,char** message, int action){//Recebe o endereço para onde vai enviar, o ip para onde vai enviar e a mensagem que envia
    int fd;
    long n;
    socklen_t addrlen;
    struct sockaddr_in addr;
    char buffer[128];

    fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
    if(fd==-1)exit(1);//error

    memset((void*)&addr,(int)'\0',sizeof(addr));

    addr.sin_family=AF_INET;
    addr.sin_addr=ip;
    addr.sin_port=htons(port);

    n=sendto(fd,*message,strlen(*message)+1,0,(struct sockaddr*)&addr,sizeof(addr));
    if(n==-1)exit(1);//error

    addrlen=sizeof(addr);
    printf("On udp socket...\n");
    n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1)exit(1);//error

    write(1,"Message received: ",18);//stdout
    write(1,buffer,n);
    printf("\n");
    close(fd);
    if (action==1){
            printf("Action = 1\n");
            return strtoelem(buffer);   // Caso do find
    }else{return NULL;}

}


element * strtoelem(char buffer[]) {
    long a_port;
    element * p_element, a_element;
    char * a_name, *a_surname, *a_ip, *end, command[64], info[64];

    printf("%s", buffer);
    if(strncmp(buffer, "INFO", 4)==0) {
        sscanf(buffer,"%s %s", command, info);
        printf("%s %s", command, info);
        a_name = (char*)strtok(info, ".");
        a_surname = (char*)strtok(NULL, ";");
        a_ip = (char*)strtok(NULL, ";");
        a_port = strtol((char*)strtok(NULL, ";"),&end,10);
        a_element = infotoelement(a_name, a_surname, a_ip, a_port);

        p_element = &a_element;
        printf("To be added to list: %s.%s;%s;%lu\n",a_name, a_surname, a_ip,a_port);
        return p_element;

    }else {return NULL;}
}


