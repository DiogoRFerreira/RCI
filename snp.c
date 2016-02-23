#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "gethostsname.h"
#include "list.h"
#include "udp_socket_client.h"
#include  <sys/types.h> //fork include

int main(int argc, char * argv[]){

    int i,j,bad_arguments=0, contagem=0; //Integers to check the arguments
    int porto_maquina = 0,porto_servidor = 0;//Port variables
    char surname[20], ip_maquina[20], ip_servidor[20];//IP variables
    char * name_ip_servidor = (char*)malloc(126*sizeof(char));//-------Falta fazer o free------//
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
        ipaddress2 = getaddressbyname(&name_ip_servidor);//Converte from name to ip
        porto_servidor = 58000;

        if(contagem != 3){bad_arguments=1;}//Wrong arguments
    }
    //All arguments
    else if(argc==11){
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
                if(inet_pton(AF_INET,argv[i+1],&(ipaddress.s_addr))==1){//Check if the ip is valid
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
    
    //Verify the address of the host
    struct in_addr compare_address;
    compare_address = get_host_name();//------------Falta comparar---------//

    //Missing arguments , too many arguments or bad arguments
    if((argc!=7 && argc!=11) || bad_arguments == 1){
        printf("Invocar aplicação da seguinte forma ./snp -n surname -s snpip -q snpport [-i saip] [-p saport]\n");
        exit(1);
    }


    //Printf dos valores
    printf("SNP:\nSurname: %s\nIP address: %s\nPort: %d\n\n",surname,ip_maquina,porto_maquina);
    printf("SA:\nIP address: %s\nPort: %d\n\n",ip_servidor,porto_servidor);

    //Registo do servidor de nomes,envio da sua localizaçã para o servidor de apelidos
    
    //utilizar o sscanf para criar a mensagem
    strcpy(message,"");
    udp_socket(ipaddress2,porto_servidor,&message);
    
    //Menu Aqui
    
    
    
    //Remover o servidor de nomes proprios do de apelidos
    
    //utilizar o sscanf para criar a mensagem
    strcpy(message,"");
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
                ptr_to_first = add_element(n_name,n_surname, n_ip, ptr_to_first);
                break;
        }
    }*/
       exit(0);
}
