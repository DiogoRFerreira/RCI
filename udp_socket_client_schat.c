#include "udp_socket_client_schat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


void udp_socket(struct in_addr ip,int port,char** message){//Recebe o endereço para onde vai enviar, o ip para onde vai enviar e a mensagem que envia
    int fd, n;
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

    n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1)exit(1);//error

    write(1,"Message received: ",18);//stdout
    write(1,buffer,n);
    printf("\n");
    close(fd);
    return;

}

void udp_socket_server(){

    int fd, ret, nread;
    struct sockaddr_in addr;
    char buffer[128];
    socklen_t addrlen;

    if((fd=socket(AF_INET,SOCK_DGRAM,0))==-1)exit(1);//error

    memset((void*)&addr,(int)'\0',sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(9000);

    ret=bind(fd,(struct sockaddr*)&addr,sizeof(addr)); if(ret==-1)exit(1);//error

    while(1){addrlen=sizeof(addr);
        nread=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
        if(nread==-1)exit(1);//error

        //------Registo do utilizador----
        //UNR name.surname
        //REG name.surname;ip;scport
        //------Query------
        //QRY name.surname

        ret=sendto(fd,buffer,nread,0,(struct sockaddr*)&addr,addrlen);
        if(ret==-1)exit(1);//error
    }
    close(fd);
}


