#include "gethostsname.h"
#include "list.h"
#include "udp_socket_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include  <sys/types.h> //fork include
#include <sys/time.h> //select
#include <unistd.h> //select

//	Error code (system calls):
//				-1:	Bad Arguments
//				 1: Socket
//				 2: Bind
//				 3: Select
// 				 4: Sendto [Function: udp_socket_server, file: udp_socket_client.c]

#define max(A,B) ((A)>=(B)?(A):(B))
int main(int argc, char * argv[]){
	//Pointer to
    element * ptr_to_first=NULL;
    //Integers to check the arguments
    int i,j,bad_arguments=0, contagem=0;
    //Port variables
    int snp_port = 0,porto_servidor = 0;
    //IP variables, Surname, Message
    char * ip_servidor = (char*)malloc(20*sizeof(char));
    char * ip_maquina = (char*)malloc(20*sizeof(char));
    char * surname = (char*)malloc(30*sizeof(char));
    char * name_ip_servidor = (char*)malloc(126*sizeof(char));
    char * message = (char*)malloc(126*sizeof(char));
	//Addresses
    struct in_addr ipaddress, ipaddress_sa;
    //Check the application's arguments
    if(argc==7){//saip and saport omitted, argc=7 (snp -n surname -s snpip -q snpport)
        for(i=1;i<7;i+=2){
            if(strcmp(argv[i],"-n")==0){//Surname
                if(strlen(argv[i+1])>20){//Check the lenght of the surname
                    printf("Invalid Surname: Too big...\n");
                    bad_arguments=1;
                }else{
                    if(argv[i+1][0]>'Z'|| argv[i+1][0]<'A'){//First letter must be a capital
                        printf("Invalid Surname: Must use a capital letter for the first character of a surname\n");
                        bad_arguments=1;
                    }else{
                        for(j=1;j<=(strlen(argv[i+1])-1);j++){//Must be lower case
                            if('a'>argv[i+1][j] || argv[i+1][j]>'z'){
                                printf("Invalid Surname: Must use lower case letters to the rest of the surname \n");
                                bad_arguments=1;
                            }
                        }
                        strncpy(surname, argv[i+1],strlen(argv[i+1]));
						printf("%s\n",surname);
                        contagem++;
                    }
                }
            }else if(strcmp(argv[i],"-s")==0){//IP address
                if(inet_pton(AF_INET,argv[i+1],&(ipaddress.s_addr))==1){//Check if the ip is valid
                    strcpy(ip_maquina, argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid IP Address\n");
                }
            }else if(strcmp(argv[i],"-q")==0){//UDP Port, Port value must be between 1024 and 65535 (maxvalue is 65535 2^16-1 16 bits)  1024–49151 registed ports 49151-65535 are called dynamic and/or private ports
                if((49151 < atoi(argv[i+1])) && (atoi(argv[i+1])<= 65535)){//Check if the port is valid
                    snp_port=atoi(argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid Port: Value of Port must be between 49151 and 65535\n");
                    bad_arguments=1;
                }
            }
        }
        //Get the address of Tejo
        strcpy(name_ip_servidor,"tejo.tecnico.ulisboa.pt");//Omitted values
        ipaddress_sa = getaddressbyname(&name_ip_servidor);//Convert from name to ip - gethostsname.c
        porto_servidor = 58000;
        if(contagem != 3){bad_arguments=1;}//Wrong arguments

    //All arguments, argc=11
    }else if(argc==11){
        for(i=1;i<11;i+=2){
            if(strcmp(argv[i],"-n")==0){//Surname
                if(strlen(argv[i+1])>20){//Check the lenght of the surname
                    printf("Invalid Surname: Too big...\n");
                    bad_arguments=1;
                }else{
                    if(argv[i+1][0]>'Z'|| argv[i+1][0]<'A'){//First letter must be a capital
                        printf("Invalid Surname: Must use a capital letter for the first character of a surname\n");
                        bad_arguments=1;
                    }else{
                        for(j=1;j<=(strlen(argv[i+1])-1);j++){//Must be lower case
                            if('a'>argv[i+1][j] || argv[i+1][j]>'z'){
                                printf("Invalid Surname: Must use lower case letters to the rest of the surname \n");
                                bad_arguments=1;
                            }
                        }
                        strncpy(surname, argv[i+1],strlen(argv[i+1]));
                        contagem++;
                    }
                }
            }else if(strcmp(argv[i],"-s")==0){//IP address SNP
                if(inet_pton(AF_INET,argv[i+1],&(ipaddress.s_addr))==1){//Check if the ip is valid
                    strcpy(ip_maquina, argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid IP Address\n");
                }
            }else if(strcmp(argv[i],"-q")==0){//UDP Port SNP
                if((49151 < atoi(argv[i+1])) && (atoi(argv[i+1])<= 65535)){//Check if the port is valid
                    snp_port=atoi(argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid Port: Value of Port must be between 49151 and 65535\n");
                    bad_arguments=1;
                }
            }else if(strcmp(argv[i],"-i")==0){//IP address SA
                if(inet_pton(AF_INET,argv[i+1],&(ipaddress_sa.s_addr))==1){//Check if the ip is valid
                    strcpy(ip_servidor, argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid IP Address\n");
                }
            }else if(strcmp(argv[i],"-p")==0){//Port SA
                if((49151 < atoi(argv[i+1])) && (atoi(argv[i+1])<= 65535)){//Check if the port is valid
                    porto_servidor=atoi(argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid Port: Value of Port must be between 49151 and 65535\n");
                    bad_arguments=1;
                }
            }
        }
        if(contagem != 5){bad_arguments=1;}//Wrong arguments
    }

    //Missing arguments , too many arguments or bad arguments
    if((argc!=7 && argc!=11) || bad_arguments == 1){
        printf("Start the program as follows: ./snp -n surname -s snpip -q snpport [-i saip] [-p saport]\n");
        exit(-1);
    }
    //Values
    printf("SNP:\nSurname: %s\nIP address: %s\nPort: %d\n\n",surname,ip_maquina,snp_port);
    printf("SA:\nIP address: %s\nPort: %d\n\n",inet_ntoa(ipaddress_sa),porto_servidor);

    //-----------------------------------Menu------------------------------------------------//
    struct sockaddr_in addr;
    int fd_file,fd_socket_schat, num_elements=0, exit_menu=0;
    int *num_elements_ptr= &num_elements;
    fd_set readset;
    char option[32];
    
    printf("\nWelcome to the Name Server Interface. Choose an action: \n\n 1: list\n 2: exit \n");
	//------Open socket to communicate with Schat's------
    if((fd_socket_schat = socket(AF_INET,SOCK_DGRAM,0))==-1){
		perror("Socket: "); exit(1);
	}
    memset((void*)&addr,(int)'\0',sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(snp_port);
    //Bind
    if(bind(fd_socket_schat,(struct sockaddr*)&addr,sizeof(addr))==-1){
		perror("Bind failed: "); exit(2);
	}
	//----------------------------------------------
    //Regist of the SNP on SA
    sprintf(message,"SREG %s;%s;%d",surname,ip_maquina,snp_port);
    udp_socket(ipaddress_sa,porto_servidor,&message);//udp_socket_client.c
	printf("Registed on SA. Waiting for connections ...\n");
	//----------------------------------------------
	//Main cycle: waiting for activity on socket or input from keyboard
    while(exit_menu==0){
        fd_file = fileno(stdin);
        FD_ZERO(&readset);
        FD_SET(fd_socket_schat,&readset);
        FD_SET(fileno(stdin), &readset);
        if(select(max(fd_file,fd_socket_schat)+1,&readset,NULL,NULL,NULL)==-1){
			perror("Error in select(): ");  exit(3); //Error in select
		}
		//Activity on keyboard
        if(FD_ISSET(fd_file,&readset)) {
            fgets(option,32,stdin);
            if (strncmp(option,"exit",4)==0){
                printf("Server terminating...\n");
                exit_menu=1;
            }else{
                if(strncmp(option,"list",4)==0){
                    printf("List: [%d]\n", num_elements);
                    printList(ptr_to_first);
                }else {
                    printf("Command not found.\n");
                }
            }
        } 
		// Activity on socket
        if(FD_ISSET(fd_socket_schat,&readset)) {
            ptr_to_first = udp_socket_server(ptr_to_first,num_elements_ptr, fd_socket_schat,addr,ipaddress_sa);//udp_socket_client.c
        }
	}
	//---------------------------------------------------------------------------------//
    //------Remove SNP from SA-----//
    sprintf(message,"SUNR %s",surname);
    udp_socket(ipaddress_sa,porto_servidor,&message);//udp_socket_client.c
    //Frees
    free(message); free(name_ip_servidor); free(surname); free(ip_servidor); free(ip_maquina);
    freeList(ptr_to_first);
	//Exit
    exit(0);
}

