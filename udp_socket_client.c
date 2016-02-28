#include "udp_socket_client.h"
#include "list.h"
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

element * udp_socket_server(element * ptr_to_first, int * num_elements_ptr, int fd, struct sockaddr_in addr, socklen_t addrlen){

    int ret, nread, prev_num_elements=*num_elements_ptr;
    element *previous_ptr_to_first; element elem;
    previous_ptr_to_first = ptr_to_first;

    char buffer[128];


 addrlen=sizeof(addr);
        nread=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
        if(nread==-1)exit(1);//error

        char message_out[64], user_input[20], *name, *surname, *ip;
    char cmpx_string[100], *parte, *end;
    int port;

    sscanf(buffer,"%s %s", user_input, cmpx_string);

        if (strcmp(user_input,"exit")==0){
            exit(5);
        }else{
            if(strcmp(user_input,"UNR")==0) {
                name = (char*)strtok(cmpx_string, ".");
                printf("%s\n", name);
                surname = (char*)strtok(NULL, ";");
                printf("%s\n", surname);
                ptr_to_first = deleteElement(ptr_to_first, name, surname, num_elements_ptr);
                if (*num_elements_ptr!=prev_num_elements) {
                    printf("Unregistered!\n");
                    ret=sendto(fd,"OK\n",strlen("OK\n")+1,0,(struct sockaddr*)&addr,addrlen);
                }else{
                    ret=sendto(fd,"NOK - Not in the list\n",strlen("NOK - Not in the list\n")+1,0,(struct sockaddr*)&addr,addrlen);
                }
            }else{
                    if(strcmp(user_input,"REG")==0){
                        name = (char*)strtok(cmpx_string, ".");
                        printf("%s\n", name);
                        surname = (char*)strtok(NULL, ";");
                        printf("%s\n", surname);

                        ip = (char*)strtok(NULL, ";");
                        printf("%s\n", ip);
                        port = strtol((char*)strtok(NULL, ";"),&end,10);
                        printf("%d\n", port);

                        ptr_to_first=addElement(ptr_to_first,infotoelement(name,surname,ip,port), num_elements_ptr);
                        if(*num_elements_ptr!=prev_num_elements){
                        printf("Registered!\n");
                        ret=sendto(fd,"OK\n",strlen("OK\n")+1,0,(struct sockaddr*)&addr,addrlen);
                        }else{
                             ret=sendto(fd,"NOK - Name and Surname already registered\n",strlen("NOK - Name and Surname already registered\n")+1,0,(struct sockaddr*)&addr,addrlen);
                        }
                    }else {
                        ret=sendto(fd,"NOK - Command not found\n",strlen("NOK - Command not found\n")+1,0,(struct sockaddr*)&addr,addrlen);
                        printf("NOK - Command not found\n");
                    }
            }
        }
        //ret=sendto(fd,buffer,nread,0,(struct sockaddr*)&addr,addrlen);
        if(ret==-1)exit(1);

    return ptr_to_first;
}


