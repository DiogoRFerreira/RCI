#include "list.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
int main(void)
{
    element * ptr_to_first=NULL; element elem;
    int fd, ret,ret2, nread;
    struct sockaddr_in addr;
    socklen_t addrlen;
    char buffer[128];

    if((fd=socket(AF_INET,SOCK_DGRAM,0))==-1)exit(1);//error

    memset((void*)&addr,(int)'\0',sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(9000);
    ret=bind(fd,(struct sockaddr*)&addr,sizeof(addr));

    if(ret==-1)exit(1);//error

    while(1){addrlen=sizeof(addr);
         nread=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
         if(nread==-1)exit(1);//error


    char message_out[64], user_input[20], *name, *surname, *ip;
    char cmpx_string[100], *parte, *end;
    int port;


    sscanf(buffer,"%s %s", user_input, cmpx_string);

        if (strcmp(user_input,"exit")==0){
            exit(5);
        }else{
                if(strcmp(user_input,"list")==0){
                    strcpy(message_out,"Listing...\n");
                    ret=sendto(fd,message_out,strlen(message_out)+1,0,(struct sockaddr*)&addr,addrlen);
                    print_list(ptr_to_first, fd, addr,addrlen);
                }else {
                        if(strcmp(user_input,"UNR")==0) {
                        name = (char*)strtok(cmpx_string, ".");
                        printf("%s\n", name);
                        surname = (char*)strtok(NULL, ";");
                        printf("%s\n", surname);
                            printf("Unregistered!\n");
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
                                    ptr_to_first=addElement(ptr_to_first,infotoelement(name,surname,ip,port),fd,buffer, addr, addrlen);
                                    if(ptr_to_first!=NULL){
                                    printf("Registered!\n");
                                    ret=sendto(fd,"OK\n",strlen("OK\n")+1,0,(struct sockaddr*)&addr,addrlen);
                                    }
                                }else{
                                    ret=sendto(fd,"NOK - Command not found\n",strlen("NOK - Command not found\n")+1,0,(struct sockaddr*)&addr,addrlen);
                                }
                        }
                }
            }

        //ret=sendto(fd,buffer,nread,0,(struct sockaddr*)&addr,addrlen);
        if(ret==-1)exit(1);

        //error
     }

    //close(fd);
    //exit(0);
}
