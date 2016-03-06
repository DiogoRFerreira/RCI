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



element * udp_socket2(struct in_addr ip,int port,char** message){//Recebe o endereço para onde vai enviar, o ip para onde vai enviar e a mensagem que envia
    int fd, n;
    socklen_t addrlen;
    struct sockaddr_in addr;
    char buffer[128], * a_name, *a_surname, *a_ip, *end, command[64], info[64];
    int a_port;
    element * p_element, a_element;

    fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
    if(fd==-1)exit(1);//error

    memset((void*)&addr,(int)'\0',sizeof(addr));

    addr.sin_family=AF_INET;
    addr.sin_addr=ip;
    addr.sin_port=htons(port);

    n=sendto(fd,*message,strlen(*message)+1,0,(struct sockaddr*)&addr,sizeof(addr));
    if(n==-1)exit(1);//error
    printf("Message sent: %s", *message);
    addrlen=sizeof(addr);

    n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);

    if(n==-1)exit(1);//error

    write(1,"Message received from other SNP: ",strlen("Message received from other SNP: "));//stdout
    write(1,buffer,n);
    printf("\n");

    if (strncmp(buffer,"RPL ",4)==0){    // Verfify if the response from other SNP is valid
            sscanf(buffer,"%s %s", command, info);
            a_name = (char*)strtok(info, ".");
            a_surname = (char*)strtok(NULL, ";");
            a_ip = (char*)strtok(NULL, ";");
            a_port = strtol((char*)strtok(NULL, ";"),&end,10);
            a_element = infotoelement(a_name, a_surname, a_ip, a_port);

            p_element = &a_element;

            return p_element;
        }else{
        return NULL;
        }
}

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
    return;

}

element *udp_socket_sa(struct in_addr ip_sa,int port, char surname[20], char name[20]){//Recebe o endereço para onde vai enviar, o ip para onde vai enviar e a mensagem que envia
    int fd,n, r_port=0;
    socklen_t addrlen;
    struct sockaddr_in addr, addr_snp2;
    char buffer[128], command[32], info[64], message[128], * end, *r_name=NULL, *r_surname, *r_ip, *buffer_2;
    char * message_to_snp = (char*)malloc(126*sizeof(char));
    element *p_element, a_element;

    sprintf(message,"SQRY %s\n", surname);

    fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
    if(fd==-1)exit(1);//error

    memset((void*)&addr,(int)'\0',sizeof(addr));

    addr.sin_family=AF_INET;
    addr.sin_addr=ip_sa;
    addr.sin_port=htons(port);

    n=sendto(fd,message,strlen(message)+1,0,(struct sockaddr*)&addr,sizeof(addr));
    if(n==-1)exit(1);//error

    addrlen=sizeof(addr);

    n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);

    if(n==-1)exit(1);//error

    write(1,"Message received: ",18);//stdout
    write(1,buffer,n);
    printf("\n");

    if (strncmp(buffer,"RPL",3)==0){    // Verfify if the response from SA is valid, and if it is, evaluate if the name.surname was  not found
            printf("Surname not found on SA\n");
            return NULL;
    }else{
        if (strncmp(buffer,"SRPL",4)==0){             // If it was found, retrieve and return the information of the element.
            sscanf(buffer,"%s %s", command, info);

            printf("INFO: %s\n", info);
            r_surname=(char*)strtok(info, ";");
            r_ip=(char*)strtok(NULL, ";");
            r_port = strtol((char*)strtok(NULL, ";"),&end,10);

            printf("Received from SA: %s %s : %d\n", r_surname, r_ip, r_port);

            inet_aton(r_ip, &addr_snp2.sin_addr);
            sprintf(message_to_snp,"QRY %s.%s",name,surname);


            p_element = udp_socket2(addr_snp2.sin_addr, r_port,&message_to_snp);
            return p_element;
        }else{

            return NULL;
        }
    }

}



element * udp_socket_server(element * ptr_to_first, int * num_elements_ptr, int fd, struct sockaddr_in addr, struct in_addr ip_sa){

    int ret, nread, prev_num_elements=*num_elements_ptr;
    element *previous_ptr_to_first, elem, *found_element, *a_element;
    previous_ptr_to_first = ptr_to_first;

    char buffer[128], message[128];

    socklen_t addrlen, addrlen_sa;
    addrlen=sizeof(addr);

    nread=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
    if(nread==-1)exit(1);//error

    char message_out[64], user_input[20], *name, *surname, *ip;
    char cmpx_string[100], *parte, *end;
    int port;

    struct hostent *hostp; /* client host info */
    char *hostaddrp; /* dotted decimal host addr string */
      struct sockaddr_in clientaddr; /* client addr */

    /*
     * gethostbyaddr: determine who sent the datagram
     */


        printf("server received %d/%d bytes: %s\n", strlen(buffer), nread, buffer);

    sscanf(buffer,"%s %s", user_input, cmpx_string);

       /* if (strcmp(user_input,"exit")==0){
            exit(5);
        }else{*/
            if(strcmp(user_input,"QRY")==0){
                name = (char*)strtok(cmpx_string, ".");
                surname = (char*)strtok(NULL, "\0");
                found_element=CheckInList(ptr_to_first, name, surname); // Verifica se o corrente SNP tem o nome completo pretendido
                if (found_element==NULL) {
                    //ret=sendto(fd,"Name.Surname not found on SNP. Querying SA...\n",strlen("Name.Surname not found on SNP. Querying SA...\n")+1,0,(struct sockaddr*)&addr,addrlen);
                    a_element = udp_socket_sa(ip_sa,58000, surname, name);
                    if (a_element==NULL) {
                        printf("Should have sent a message to schat!");
                        ret=sendto(fd,"Surname not found on SA\n",strlen("Surname not found on SA\n")+1,0,(struct sockaddr*)&addr,addrlen);
                    }else{
                    sprintf(message,"Surname Found on SA and SNP: %s;%s;%d\n", a_element->surname, a_element->ip, a_element->port);

                    ret=sendto(fd,message,strlen(message)+1,0,(struct sockaddr*)&addr,addrlen);
                    }
                }else{
                    printf("%s.%s;%s;%d\n", found_element->name,found_element->surname, found_element->ip, found_element->port);
                    sprintf(message,"RPL %s.%s;%s;%d\n", found_element->name,found_element->surname, found_element->ip, found_element->port);
                    ret=sendto(fd,message,strlen(message)+1,0,(struct sockaddr*)&addr,addrlen);
                    }
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


