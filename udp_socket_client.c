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

#define timeout 4

//Socket UDP Query to SNP (Destination's address, Destination's port, Message to send)
element * udp_socket_to_snp(struct in_addr ip,int port,char** message){
    int fd;
    long int n;
    socklen_t addrlen;
    struct sockaddr_in addr;
    char buffer[128], * a_name, *a_surname, *a_ip, *end, command[64], info[64];
    long int a_port;
    element * p_element;
    
	//Timeout
	struct timeval tv;
	tv.tv_sec = timeout;//Timeout time secs
	tv.tv_usec = 0;
	
	//Open UDP socket
    fd=socket(AF_INET,SOCK_DGRAM,0);
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
    if(fd==-1)exit(1);//error

    memset((void*)&addr,(int)'\0',sizeof(addr));

    addr.sin_family=AF_INET;
    addr.sin_addr=ip;
    addr.sin_port=htons(port);
	
	//Send message
    n=sendto(fd,*message,strlen(*message)+1,0,(struct sockaddr*)&addr,sizeof(addr));
    if(n==-1)exit(1);//error
    printf("Message sent: %s\n", *message);
    addrlen=sizeof(addr);
	//Receive message
    n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
	if(n==-1){
		printf("Timed out: Couldn't reach SNP given by SA.....\n");
		return NULL;
	}
    write(1,"Message received from other SNP: ",strlen("Message received from other SNP: "));//stdout
    write(1,buffer,n);
    printf("\n");

    if (strncmp(buffer,"RPL ",3)==0){// Verfify if the response from other SNP is valid
        if(sscanf(buffer,"%s %s", command, info)==2){
            a_name = (char*)strtok(info, ".");
            a_surname = (char*)strtok(NULL, ";");
            a_ip = (char*)strtok(NULL, ";");
            a_port = strtol((char*)strtok(NULL, ";"),&end,10);
            p_element = infotoelement(a_name, a_surname, a_ip, a_port);
            return p_element;
        }else{  return NULL; }
    }
    return NULL;
}

//UDP Socket (Destination's address, Destination's port, Message to send)
void udp_socket(struct in_addr ip,int port,char** message){
    int fd;
    long int n;
    socklen_t addrlen;
    struct sockaddr_in addr;
    char buffer[128];
	//Timeout
	struct timeval tv;
	tv.tv_sec = timeout;//Timeout time secs
	tv.tv_usec = 0;

    fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));

    if(fd==-1){
		exit(1);//error
	}

    memset((void*)&addr,(int)'\0',sizeof(addr));

    addr.sin_family=AF_INET;
    addr.sin_addr=ip;
    addr.sin_port=htons(port);

    n=sendto(fd,*message,strlen(*message)+1,0,(struct sockaddr*)&addr,sizeof(addr));
    if(n==-1)exit(1);//error

    addrlen=sizeof(addr);

    n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1){
		printf("Timed out: Couldn't reach SA for registering.\n");
		return;
		//exit(1);//error
	}
    write(1,"Message received: ",18);//stdout
    write(1,buffer,n);
    printf("\n");
    return;

}

// Does QRY to SA and returns an element if it exists
element * udp_socket_sa(struct in_addr ip_sa,int port, char surname[20], char name[20]){
    int fd;
    long int n, r_port=0;
    socklen_t addrlen;
    struct sockaddr_in addr, addr_snp2;
    char command[32], message[128], * end, *r_surname, *r_ip;
    char * buffer = (char*)malloc(128*sizeof(char));
    char * info = (char*)malloc(64*sizeof(char));
    char * message_to_snp = (char*)malloc(126*sizeof(char));
    element *p_element;
	//Timeout
	struct timeval tv;
	tv.tv_sec = timeout;//Timeout time secs
	tv.tv_usec = 0;

    sprintf(message,"SQRY %s\n", surname);

    fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
    if(fd==-1)exit(1);//error

    memset((void*)&addr,(int)'\0',sizeof(addr));

    addr.sin_family=AF_INET;
    addr.sin_addr=ip_sa;
    addr.sin_port=htons(port);

    n=sendto(fd,message,strlen(message)+1,0,(struct sockaddr*)&addr,sizeof(addr));
    if(n==-1)exit(1);//error

    addrlen=sizeof(addr);

    n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1){
		printf("Timed out: No response from SA to QRY\n");
		return NULL;
		//exit(1);//error
	}
    write(1,"Message received: ",18);//stdout
    write(1,buffer,n);
    printf("\n");

    // Verfify if the response from SA is valid, and if it is, evaluate if the name.surname was  not found
    if (strncmp(buffer,"SRPL",4)==0){// If it was found, retrieve and return the information of the element.
        if((sscanf(buffer,"%s %s", command, info))==2){
            r_surname=(char*)strtok(info, ";");
            r_ip=(char*)strtok(NULL, ";");
            r_port = strtol((char*)strtok(NULL, ";"),&end,10);
            printf("Received from SA: %s %s : [%lu] \n", r_surname, r_ip, r_port);
            inet_aton(r_ip, &addr_snp2.sin_addr);
            sprintf(message_to_snp,"QRY %s.%s",name,surname);
            p_element = udp_socket_to_snp(addr_snp2.sin_addr, r_port,&message_to_snp); //Query to SNP provided by SA
            return p_element;
        }else{
            printf("Surname not found on SA\n");
            return NULL;
        }
    }
    return NULL;
}

// Handles all instructions in SNP when there is activiy on Schat socket.
element * udp_socket_server(element * ptr_to_first, int * num_elements_ptr, int fd, struct sockaddr_in addr, struct in_addr ip_sa){

    int prev_num_elements=*num_elements_ptr;
    long int ret;
    element *found_element, *a_element=NULL;

    char * buffer = (char*)malloc(126*sizeof(char));
    char * message = (char*)malloc(126*sizeof(char));
    
	//Timeout
	struct timeval tv;
	tv.tv_sec = timeout;//Timeout time secs
	tv.tv_usec = 0;

    socklen_t addrlen;
    addrlen=sizeof(addr);
    
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
    //	Read from SCHAT socket. No response => Timeout!
    if(recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen)==-1){
		printf("Timed out: No response from Schat\n");
		return ptr_to_first;
	}

    char user_input[20], *name, *surname, *ip;
    char cmpx_string[100], *end;
    long int port;
	
	// Decompose incoming string from SCHAT and evaluate which protocol message it is.
    sscanf(buffer,"%s %s", user_input, cmpx_string);
    
	if(strncmp(user_input,"QRY",3)==0){
		name = (char*)strtok(cmpx_string, ".");
		surname = (char*)strtok(NULL, "\0");
		
		// Check for name.surname on SNP local list
		found_element=CheckInList(ptr_to_first, name, surname); // Verifica se o corrente SNP tem o nome completo pretendido
		//If name.surname is not on the list
		if (found_element==NULL){ 
			// Query SA and return the element if it was found
			a_element = udp_socket_sa(ip_sa,58000, surname, name);
			if (a_element==NULL) {
				printf("Surname not found on SA.\n");
				ret=sendto(fd,"Surname not found on SA\n",strlen("Surname not found on SA\n")+1,0,(struct sockaddr*)&addr,addrlen);
				return ptr_to_first;
			}else{
				sprintf(message,"RPL %s.%s;%s;%lu\n", a_element->name,a_element->surname, a_element->ip, a_element->port);
				ret=sendto(fd,message,strlen(message)+1,0,(struct sockaddr*)&addr,addrlen);
			}
		}else{
			printf("%s.%s;%s;%lu\n", found_element->name,found_element->surname, found_element->ip, found_element->port);
			sprintf(message,"RPL %s.%s;%s;%lu\n", found_element->name,found_element->surname, found_element->ip, found_element->port);
			ret=sendto(fd,message,strlen(message)+1,0,(struct sockaddr*)&addr,addrlen);
		}
	}else{
		// Handle user trying to unregister from SNP
		if(strcmp(user_input,"UNR")==0) {
			name = (char*)strtok(cmpx_string, ".");
			surname = (char*)strtok(NULL, ";");
			ptr_to_first = deleteElement(ptr_to_first, name, surname, num_elements_ptr);
			// If list was modified by deleteElement
			if (*num_elements_ptr!=prev_num_elements) {
				printf("Unregistered!\n");
				ret=sendto(fd,"OK\n",strlen("OK\n")+1,0,(struct sockaddr*)&addr,addrlen);
			}else{
				ret=sendto(fd,"NOK - Not in the list\n",strlen("NOK - Not in the list\n")+1,0,(struct sockaddr*)&addr,addrlen);
			}
		}else{
			// Handle user trying to register on SNP
			if(strcmp(user_input,"REG")==0){
				name = (char*)strtok(cmpx_string, ".");
				surname = (char*)strtok(NULL, ";");
				ip = (char*)strtok(NULL, ";");
				port = strtol((char*)strtok(NULL, ";"),&end,10);
				// Check for element in SNP list
				if(CheckInList(ptr_to_first, name,surname)==NULL){
					ptr_to_first=addElement(ptr_to_first,infotoelement(name,surname,ip,port), num_elements_ptr);
					if(*num_elements_ptr!=prev_num_elements){
						printf("Registered!\n");
						ret=sendto(fd,"OK\n",strlen("OK\n")+1,0,(struct sockaddr*)&addr,addrlen);
					}
				}else{
					ret=sendto(fd,"NOK - Name and Surname already registered\n",strlen("NOK - Name and Surname already registered\n")+1,0,(struct sockaddr*)&addr,addrlen);
				}
			}else{
				ret=sendto(fd,"NOK - Command not found\n",strlen("NOK - Command not found\n")+1,0,(struct sockaddr*)&addr,addrlen);
			}
		}
	}
    if(ret==-1){
		perror("Error in sendto: ");
		exit(4);
	}

    return ptr_to_first;
}


