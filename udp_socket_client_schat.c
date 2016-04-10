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

#define timeout 3

//Recebe o endereço para onde vai enviar, o ip para onde vai enviar e a mensagem que envia
element * udp_socket(struct in_addr ip,int port,char** message, int action){
    int fd;
    long n;
    socklen_t addrlen;
    struct sockaddr_in addr;
    char buffer[128];

	//Timeout
	struct timeval tv;
	tv.tv_sec = timeout;//Timeout time secs
	tv.tv_usec = 0;

    fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
    if(fd==-1)exit(1);//error

    memset((void*)&addr,(int)'\0',sizeof(addr));

    addr.sin_family=AF_INET;
    addr.sin_addr=ip;
    addr.sin_port=htons(port);

    n=sendto(fd,*message,strlen(*message)+1,0,(struct sockaddr*)&addr,sizeof(addr));
    if(n==-1){
		perror("Error sending data to snp: ");
		return NULL;
	}
    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1){
		printf("Timed out: Couldn't reach SNP given by SA\n");
		return NULL;
	}
    write(1,"Protocol msg rcv: ",18);//stdout
    write(1,buffer,n);
    close(fd);
    if (action==1){
      return strtoelem(buffer);   // Caso do find
    }else{return NULL;}

}
// Converts valid string to valid element 
element * strtoelem(char buffer[]) {
    long a_port;
    element * p_element;
    char * a_name, *a_surname, *a_ip, *end;
    char * command = (char*)malloc(64*sizeof(char));
     char * info = (char*)malloc(64*sizeof(char));

    printf("%s", buffer);
    if(strncmp(buffer, "RPL", 3)==0) {
        sscanf(buffer,"%s %s", command, info);
        a_name = (char*)strtok(info, ".");
        a_surname = (char*)strtok(NULL, ";");
        a_ip = (char*)strtok(NULL, ";");
        a_port = strtol((char*)strtok(NULL, ";"),&end,10);
        p_element = infotoelement(a_name, a_surname, a_ip, a_port);
        return p_element;

    }else {return NULL;}
}


