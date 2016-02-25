#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc, const char * argv[]) {
    
    int i, bad_arguments=0,contagem=0;
    int porto_schat_tcp, porto_snp_udp;

    char ip_snp[20], ip_schat[20];
    
    struct in_addr ipaddress, ipaddress2;
    
    if(argc==11){
        for(i=1;i<11;i+=2){
            if(strcmp(argv[i],"-n")==0){//name.surname
                /*if(strlen(argv[i+1])>20){//Check the lenght of the surname
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
            }*/
            }else if(strcmp(argv[i],"-s")==0){//IP address do servidor de nomes proprios associado ao apelido surname
                if(inet_pton(AF_INET,argv[i+1],&(ipaddress.s_addr))==1){        //Check if the ip is valid
                    strcpy(ip_snp, argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid SNP IP Address\n");
                }
            }else if(strcmp(argv[i],"-q")==0){//UDP Port
                if((49151 < atoi(argv[i+1])) && (atoi(argv[i+1])<= 65535)){//Check if the port is valid
                    porto_snp_udp=atoi(argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid SNP Port: Value of Port must be between 49151 and 65535\n");
                    bad_arguments=1;
                }
            }else if(strcmp(argv[i],"-i")==0){//IP address of surname's server
                if(inet_pton(AF_INET,argv[i+1],&(ipaddress2.s_addr))==1){//Check if the ip is valid
                    strcpy(ip_schat, argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid SCHAT IP Address\n");
                }
            }else if(strcmp(argv[i],"-p")==0){//Port of surname's server
                if((49151 < atoi(argv[i+1])) && (atoi(argv[i+1])<= 65535)){//Check if the port is valid
                    porto_schat_tcp=atoi(argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid SCHAT Port: Value of Port must be between 49151 and 65535\n");
                    bad_arguments=1;
                }
            }
        }
        if(contagem != 5){bad_arguments=1;}//Wrong arguments
    }
    
    //Check the host's address
    struct in_addr compare_address;
    compare_address = get_host_name();
    if(compare_address.s_addr!=ipaddress.s_addr){
        printf("SCHAT´s address doesn't correspond to the address where the application is running\n");
        printf("Try %s for IP address\n",inet_ntoa(compare_address));
        exit(1);
    }
    
    //Missing arguments , too many arguments or bad arguments
    if((argc!=11) || bad_arguments == 1){
        printf("Invocar aplicação da seguinte forma: ./schat -n name.surname -i ip -p scport -s snpip -q snpport\n");
        exit(1);
    }
    
    //Print of the values
    printf("SCHAT:\nName: %s\nIP address: %s\nPort: %d\n\n",surname,ip_schat,porto_schat_tcp);
    printf("SNP:\nIP address: %s\nPort: %d\n\n",inet_ntoa(ipaddress2),porto_snp_udp);
    
    
    //Menu
    int exit_menu=0;
    while(exit_menu==0){
        //fgets
        
        
        
        
    }
}

    
    

