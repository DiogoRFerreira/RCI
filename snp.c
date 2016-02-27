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

int getMax(int a, int b) {
    int c = a - b;
    int k = (c >> 31) & 0x1;
    int max = a - k * c;
    return max;
}

int main(int argc, char * argv[]){
    element * ptr_to_first=NULL;
    int i,j,bad_arguments=0, contagem=0; //Integers to check the arguments
    int porto_maquina = 0,porto_servidor = 0;//Port variables
    char surname[20], ip_maquina[20], ip_servidor[20];//IP variables
    
    char * name_ip_servidor = (char*)malloc(126*sizeof(char));//Cuidado com o free
    char * message = (char*)malloc(126*sizeof(char));
    
    struct in_addr ipaddress, ipaddress2;
    
    //Check the application's arguments
    
    //saip and saport omitted, argc=7 (snp -n surname -s snpip -q snpport)
    if(argc==7){
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
            }else if(strcmp(argv[i],"-q")==0){//UDP Port, valor dos portos está entre 1024 e 65535 (valor máximo é 65535 2^16-1 16 bits)  1024–49151 portos registados acima de 49151 are called dynamic and/or private ports
                if((49151 < atoi(argv[i+1])) && (atoi(argv[i+1])<= 65535)){//Check if the port is valid
                    porto_maquina=atoi(argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid Port: Value of Port must be between 49151 and 65535\n");
                    bad_arguments=1;
                }
            }
        }
        //Get the address of Tejo
        strcpy(name_ip_servidor,"tejo.tecnico.ulisboa.pt");//Omitted values
        ipaddress2 = getaddressbyname(&name_ip_servidor);//Convert from name to ip
        porto_servidor = 58000;
        
        if(contagem != 3){bad_arguments=1;}//Wrong arguments
        
        //All arguments
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
            }else if(strcmp(argv[i],"-s")==0){//IP address
                if(inet_pton(AF_INET,argv[i+1],&(ipaddress.s_addr))==1){        //Check if the ip is valid
                    strcpy(ip_maquina, argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid IP Address\n");
                }
            }else if(strcmp(argv[i],"-q")==0){//UDP Port
                if((49151 < atoi(argv[i+1])) && (atoi(argv[i+1])<= 65535)){//Check if the port is valid
                    porto_maquina=atoi(argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid Port: Value of Port must be between 49151 and 65535\n");
                    bad_arguments=1;
                }
            }else if(strcmp(argv[i],"-i")==0){//IP address of surname's server
                if(inet_pton(AF_INET,argv[i+1],&(ipaddress2.s_addr))==1){//Check if the ip is valid
                    strcpy(ip_servidor, argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid IP Address\n");
                }
            }else if(strcmp(argv[i],"-p")==0){//Port of surname's server
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
    
    //Check the host's address
    /*struct in_addr compare_address;
     compare_address = get_host_name();
     if(compare_address.s_addr!=ipaddress.s_addr){
     printf("SNP´s address doesn't correspond to the address where the application is running\n");
     printf("Try %s for IP address\n",inet_ntoa(ipaddress));
     exit(1);
     }*/
    
    //Missing arguments , too many arguments or bad arguments
    if((argc!=7 && argc!=11) || bad_arguments == 1){
        printf("Invocar aplicação da seguinte forma: ./snp -n surname -s snpip -q snpport [-i saip] [-p saport]\n");
        exit(1);
    }
    
    //Print of the values
    printf("SNP:\nSurname: %s\nIP address: %s\nPort: %d\n\n",surname,ip_maquina,porto_maquina);
    printf("SA:\nIP address: %s\nPort: %d\n\n",inet_ntoa(ipaddress2),porto_servidor);
    
    //Registo do servidor de nomes,envio da sua localizaçã para o servidor de apelidos
    sprintf(message,"SREG %s;%s;%d",surname,ip_maquina,porto_maquina);
    udp_socket(ipaddress2,porto_servidor,&message);
    
    //----------------------------------------------//
    //Menu Aqui
    struct sockaddr_in addr;
    socklen_t addrlen;
    int afd,fd_file,fd_socket,result, ret;
    fd_set readset;
    char option[32], user_input[32];
    
    int exit_menu=0;
    
    printf("\nWelcome to the Name Server Interface. Choose an action: \n 1: List\n 2: Exit \n");
    
    
    if((fd_socket=socket(AF_INET,SOCK_DGRAM,0))==-1)exit(1);//error
    
    memset((void*)&addr,(int)'\0',sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);
    addr.sin_port=htons(50000);
    
    //Bind
    ret=bind(fd_socket,(struct sockaddr*)&addr,sizeof(addr));
    if(ret==-1)exit(1);
    
    addrlen = sizeof(addr);
    printf("Waiting for connections ...\n");
    element * previous_ptr_to_first = ptr_to_first;
    
    while(exit_menu==0){
        fd_file = fileno(stdin);
        FD_ZERO(&readset);
        FD_SET(fd_socket,&readset);
        FD_SET(fileno(stdin), &readset);
        
        result = select(fd_file+fd_socket+1,&readset,NULL,NULL,NULL);
        if(result==-1)exit(1);//error
        
        printf("Result: %d\n", result);
        
        if(FD_ISSET(fd_file,&readset)){
            fgets(option,32,stdin);
            sscanf(option, "%s", user_input);
            if (strcmp(user_input,"exit")==0){
                printf("Server terminating...\n");
                exit_menu=1;
            }else{
                if(strcmp(user_input,"list")==0){
                    printf("List:\n\n");
                    printList(ptr_to_first);
                }else {
                    printf("Command not found!\n");
                }
            }
        }
        if(FD_ISSET(fd_socket,&readset)){
            ptr_to_first=udp_socket_server(fd_socket, addr, addrlen);
            if (previous_ptr_to_first!=ptr_to_first) {
                printf("Pointer has changed with NEW REGISTER!\n");
            }
        }
    }
    
    
    //-----------------------------------------------//
    
    //------Remover o servidor de nomes proprios do de apelidos-----
    sprintf(message,"SUNR %s",surname);
    udp_socket(ipaddress2,porto_servidor,&message);
    
    /*
     
     // Linked list with elements with Full Name <--> IP address
     element *ptr_to_first;* new,
     char n_name[20], n_surname[20], n_ip[20];
     ptr_to_first = NULL;
     
     // Interface with the user
     int interface_option;
     int exit_menu=0;
     while(exit_menu==0){
     
     printf("\nWelcome to the Name Server Interface. Choose an action: \n 1: List\n 2: Exit \n 3: [TESTING] Add to list \n Action nr: ");
     scanf("%d",&interface_option);
     
     switch (interface_option) {
     case 1:
     printf("\n Complete table of association [Full name] <--> [IP]<\n");
     print_list(ptr_to_first);
     break;
     case 2:
     printf("\n Terminating server... \n");
     exit_menu=1;
     break;
     // NOT PART OF THE INTERFACE. Just for testing the linked list.
     case 3:
     printf("\n Write in format [Name Surname IP] to be added: ");
     scanf("%s", n_name);
     scanf("%s", n_surname);
     scanf("%s", n_ip);
     ptr_to_first = addElement(n_name,n_surname, n_ip, ptr_to_first);
     break;
     case 4:
     printf("\n Write in format to delete from list [Name Surname]: ");
     scanf("%s", n_name);
     scanf("%s", n_surname);
     ptr_to_first = deleteElement(ptr_to_first, n_name, n_surname);
     break;
     }
     }*/
    
    //Frees
    free(message);
    
    exit(0);
}

