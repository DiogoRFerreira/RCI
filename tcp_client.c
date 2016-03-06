#include "tcp_client.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


void tcp_connect(struct in_addr ip,int port) {
    
    fd_set readset;
    int fd_tcp, n, disconnect=0, fd_file, chat=0, result;
    long nbytes,nleft, nwritten, nread;
    char *ptr, buffer[128], message_received[128], message_chat[128], line[128], menu[128], menu2[128];
    struct sockaddr_in addr;

    
  fd_tcp=socket(AF_INET,SOCK_STREAM,0);//TCP socket
  if(fd_tcp==-1)exit(1);//error

  memset((void*)&addr,(int)'\0',sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr=ip;
  addr.sin_port=htons(port);

  n=connect(fd_tcp,(struct sockaddr*)&addr,sizeof(addr));
  if(n==-1)exit(1);//error

  while(disconnect!=1){

    fd_file = fileno(stdin);
    FD_ZERO(&readset);
    FD_SET(fd_tcp,&readset);
    FD_SET(fileno(stdin), &readset);

    result = select(fd_file+fd_tcp+1,&readset,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
    if(result==-1)exit(1);//error


    if(FD_ISSET(fd_file,&readset)){
        fgets(line,128,stdin);
        sscanf(line,"%s",menu);

        if(strncmp(menu, "disconnect", strlen("disconnect"))==0){
          printf("Disconnecting...\n");
          disconnect=1;
        } else if(strncmp(menu, "message", strlen("message"))==0) {
          sscanf(line,"%s %s",menu2, message_chat);
          chat = 1;

          ptr=strcpy(buffer,message_chat);
          nbytes=strlen(message_chat);
          nleft=nbytes;
          while(nleft>0){
            nwritten=write(fd_tcp,ptr,nleft);
            if(nwritten<=0)exit(1);//error
            nleft-=nwritten;
            ptr+=nwritten;
          }

          nleft=nbytes; ptr=buffer;

        }else{
          printf("Wrong command. Neither message or disconnect\n");
        }

    }

    if(FD_ISSET(fd_tcp,&readset)){
      if((nread=read(fd_tcp,message_received,128))!=0){
          if(nread==-1)exit(1);//error
          printf("Message received: %s",message_received);
      }

    }
  }

}
