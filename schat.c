#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "udp_socket_client_schat.h"
#include "list.h"
#include "tcp_client.h"
//#include "gethostsname.h"
#include <signal.h>

int main(int argc, const char * argv[]) {

    int i, j,bad_arguments=0,contagem=0;
    int porto_schat_tcp=0, porto_snp_udp = 0;

    char ip_snp[20], ip_schat[20], keyfile[64], name_surname[64] ;
    char * name,*surname, user_input[64];

    struct in_addr ipaddress, ipaddress_snp;

    if(argc==11){
        for(i=1;i<11;i+=2){
            if(strcmp(argv[i],"-n")==0){//name.surname
                sscanf(argv[i+1],"%s",user_input);
                name = (char*)strtok(user_input, ".");
                surname = (char*)strtok(NULL, " ");
                printf("Name: %s Surname: %s\n", name, surname);
                if(strlen(name)>20 || strlen(surname)>20){//Check the lenght of the name and the surname
                    printf("Invalid Name/Surname: Too big...\n");
                    bad_arguments=1;
                }else{
                    if(name[0]>'Z'|| name[0]<'A'||surname[0]>'Z'||surname[0]<'A'){//First letter must be a capital
                        printf("Invalid Name/Surname: Must use a capital letter for the first character of a surname/name\n");
                        bad_arguments=1;
                    }else{
                        for(j=1;j<=(strlen(name)-1);j++){//Must be lower case
                            if('a'>name[j] || name[j]>'z'){
                                printf("Invalid Name: Must use lower case letters to the rest of the surname \n");
                                bad_arguments=1;
                            }
                        }
                        for(j=1;j<=(strlen(surname)-1);j++){//Must be lower case
                            if('a'>surname[j] || surname[j]>'z'){
                                printf("Invalid Surname: Must use lower case letters to the rest of the surname \n");
                                bad_arguments=1;
                            }
                        }
                        contagem++;
                    }
                }
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
                if(inet_pton(AF_INET,argv[i+1],&(ipaddress_snp.s_addr))==1){//Check if the ip is valid
                    strcpy(ip_schat, argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid SCHAT IP Address\n");
                    bad_arguments=1;
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
    /*struct in_addr compare_address;
     compare_address = get_host_name();
     if(compare_address.s_addr!=ipaddress.s_addr){
     printf("SCHAT´s address doesn't correspond to the address where the application is running\n");
     printf("Try %s for IP address\n",inet_ntoa(compare_address));
     exit(1);
     }
     */
    //Missing arguments , too many arguments or bad arguments
    if((argc!=11 || bad_arguments == 1)){
        printf("Invocar aplicação da seguinte forma: ./schat -n name.surname -i ip -p scport -s snpip -q snpport\n");
        exit(1);
    }

    //Print of the values
    printf("SCHAT:\nName: %s\nIP address: %s\nPort: %d\n\n",surname,ip_schat,porto_schat_tcp);
    printf("SNP:\nIP address: %s\nPort: %d\n\n",/*inet_ntoa(ipaddress_snp)*/ip_snp,porto_snp_udp);

    //---------Socket Server e o Menu------//
    //SIGPIPE signal
    void (*old_handler)(int);//interrupt handler
    if((old_handler=signal(SIGPIPE,SIG_IGN))==SIG_ERR)exit(1);//error


    fd_set readset;
    struct sockaddr_in addr;
    socklen_t addrlen;
    int exit_menu=0,fd_file, result, fd_tcp, msg_rcv_f=0, nbytes, nleft, nw;
    long newfd, afd ,nwritten ,n;
    char menu[128], menu2[64], line[128], *a_name, *a_surname, msg_w[128], *ptr, buffer[128];
    enum {idle,busy} state;

    char * message = (char*)malloc(126*sizeof(char));

    //TCP Server bind, listen
    fd_tcp=socket(AF_INET,SOCK_STREAM,0);//TCP socket
    if(fd_tcp==-1)exit(1);//error
    memset((void*)&addr,(int)'\0',sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);//Any address can connect
    addr.sin_port=htons(porto_schat_tcp);//Tcp schat port
    //Bind
    if(bind(fd_tcp,(struct sockaddr*)&addr,sizeof(addr))==-1)
        exit(1);//error
    //Listen
    if(listen(fd_tcp,5)==-1)exit(1);//error
    state = idle;
    addrlen=sizeof(addr);
    while(exit_menu==0){

        fd_file = fileno(stdin);

        FD_ZERO(&readset);
        FD_SET(fd_tcp,&readset);
        FD_SET(fileno(stdin), &readset);

        result = select(fd_file+fd_tcp+1,&readset,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
        if(result==-1)exit(1);//error

        if(FD_ISSET(fd_file,&readset)){
            fgets(line,128,stdin);
            sscanf(line,"%s",menu);
            if(strcmp(menu,"join")==0){//-------Join-------//
                sprintf(message,"REG %s.%s;%s;%d",name,surname, ip_schat,porto_schat_tcp);
                printf("Message sent: %s", message);
                udp_socket(ipaddress_snp, porto_snp_udp,&message);
            }else if(strcmp(menu,"leave")==0){//------Leave-------//
                sprintf(message,"UNR %s.%s",name,surname);
                udp_socket(ipaddress_snp, porto_snp_udp,&message);
            }else if(strcmp(menu,"exit")==0){//------Exit-------//
                close(fd_tcp);
                exit_menu=1;
            }else if(strncmp(menu,"disconnect",strlen("disconnect"))==0){//------Disconnect-------//
                if(state==busy) {
                    close(afd);
                    state=idle;
                }else{
                    printf("No schat to disconnect from! (Not connected to any schat)\n");
                }
            }else if(strncmp(menu,"message",strlen("message"))==0){
                if (state == busy){
                    sscanf(line,"%s %s", menu, msg_w);
                    msg_rcv_f = 1;
                }else {
                    printf("Not connected to any schat\n");
                }
            }else{
              printf("Line: %s\n", line);
              sscanf(line,"%s %s %s",menu2, name_surname, keyfile);
             // printf("Find [MENU|NAMESURNAME|KEYFILE]:   %s %s %s\n", menu2, name_surname, keyfile);

              a_name=(char*)strtok(name_surname, ".");
              a_surname=(char*)strtok(NULL, " ");

              if(strncmp(menu,"connect", strlen("connect"))==0){
                //sscanf(line,"%s %s",menu2, message_chat);
                //Verificar se está na lista, usar udp_socket com retorno do elemento
                // Verificar se já está ligado a alguém
                struct in_addr ipaddress_connect;
                inet_pton(AF_INET,"192.168.1.3",&(ipaddress_connect.s_addr));
                tcp_connect(ipaddress_connect, 51001);

              }else if(strcmp(menu2,"find")==0){    //-----Find----//
                  //printf("Find [NAME.SURNAME]:   %s.%s\n", a_name,a_surname);
                  sprintf(message,"QRY %s.%s",a_name,a_surname);
                  udp_socket(ipaddress_snp, porto_snp_udp,&message);

              }else{

              printf("Wrong command...\n");
              }

            }
        }
        if(FD_ISSET(fd_tcp,&readset)){ //Socket tcp
          if((newfd=accept(fd_tcp,(struct sockaddr*)&addr,&addrlen))==-1)exit(1);//error
            switch(state)
            {
              case idle: afd = newfd; state=busy; break;
              case busy: ptr=strcpy(buffer,"Busy!\n");
                        nbytes=6;
                        nleft=nbytes;
                        while(nleft>0){
                            nwritten=write(fd_tcp,ptr,nleft);
                            if(nwritten<=0)exit(1);//error
                            nleft-=nwritten;
                            ptr+=nwritten;
                        }
              close(newfd); break;
            }
        }

        if(FD_ISSET(afd,&readset)) {
           if((n=read(afd,buffer,128))!=0){
              if(n==-1)exit(1);//error
                write(1,"Message received: ", 18);
                write(1, buffer, n);
           }else{
              if(msg_rcv_f==1){
                if((nw=write(afd,msg_w,128))<=0){
                    if (nw<=0)exit(1); //error
                    msg_rcv_f=0;
                }
              }
             close(afd); state=idle;
           }//connection closed by peer
        }
    }
    //Fre
    free(message);

    exit(0);
}








