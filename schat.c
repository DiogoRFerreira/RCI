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
    char * name=NULL,*surname=NULL, user_input[64];

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
    int exit_menu=0,fd_file, result, fd_tcp, afd=0, msg_rcv_f=0, newfd, join_f=0; long nbytes, nleft, nwritten, n;
    char menu2[64], line[128], *a_name, *a_surname, *ptr, buffer[128], *ptr3, *keyfile_p;
    enum {idle,busy} state;
    element * p_element=NULL, *ptr_to_first=NULL;
    int num_elements=0;
    int *num_elements_ptr= &num_elements;
    //int prev_num_elements=*num_elements_ptr;
    int auth_f=0, auth_success=0;
    //prev_num_elements=0;

    char * message = (char*)malloc(126*sizeof(char));

    //TCP Server bind, listen
    fd_tcp=socket(AF_INET,SOCK_STREAM,0);//TCP socket
    if(fd_tcp==-1)exit(1);//error
    memset((void*)&addr,(int)'\0',sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);//Any address can connect
    addr.sin_port=htons(porto_schat_tcp);//Tcp schat port
    //Bind
    printf("Porto de listen: %d\n", porto_schat_tcp);
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
        if(state==busy){FD_SET(afd,&readset);}
        FD_SET(fd_file, &readset);

        if(state==busy){result = select(fd_file+fd_tcp+afd+1,&readset,NULL,NULL,NULL);
        }else{result = select(fd_file+fd_tcp+1,&readset,NULL,NULL,NULL);}

        if(result==-1)exit(1);//error

        if(FD_ISSET(fd_file,&readset)){
            printf("--Reading from keyboard...\n");
            fgets(line,128,stdin);
            printf("line: %s\n",line);
            if((strncmp(line,"join",strlen("join"))==0) && (join_f == 0)){//-------Join-------//
                sprintf(message,"REG %s.%s;%s;%d",name,surname, ip_schat,porto_schat_tcp);
                printf("Message sent: %s", message);
                udp_socket(ipaddress_snp, porto_snp_udp,&message, 0);
                join_f = 1;
            }else if((strncmp(line,"leave", strlen("leave"))==0) && (join_f==1)){//------Leave-------//
                sprintf(message,"UNR %s.%s",name,surname);
                udp_socket(ipaddress_snp, porto_snp_udp,&message,0);
                join_f = 0;
            }else if(strcmp(line,"exit")==0){//------Exit-------//
                close(fd_tcp);
                exit_menu=1;
            }else if(strncmp(line,"disconnect",strlen("disconnect"))==0){//------Disconnect-------//
                if(state==busy) {
                    close(afd);
                    state=idle;
                }else{
                    printf("No schat to disconnect from! (Not connected to any schat)\n");
                }
            }else if(strncmp(line,"message",strlen("message"))==0){
                if (state == busy){
                    ptr3=(char*)strtok(line, " ");
                    ptr3=(char*)strtok(NULL, "\0");
                    //sscanf(line,"%s %s", menu, msg_w);
                     printf("Message detected to be sent: %s\n", ptr3);
                    msg_rcv_f = 1;

                    if (msg_rcv_f==1){
                      ptr=strcpy(buffer,ptr3); nbytes=strlen(ptr3); nleft=nbytes;
                      printf("--writing message to other party...\n");
                      while(nleft>0){
                        printf("...");
                        nwritten=write(afd,ptr,nleft);
                        if(nwritten<=0)exit(1);//error
                        nleft-=nwritten;  ptr+=nwritten;
                      }
                      printf("\n");
                      nleft=nbytes; ptr=buffer;
                      msg_rcv_f=0;
                  }



                }else {
                    printf("Not connected to any schat\n");
                }
            }else {
              printf("Line: %s\n", line);
              sscanf(line,"%s %s %s",menu2, name_surname, keyfile);
             // printf("Find [MENU|NAMESURNAME|KEYFILE]:   %s %s %s\n", menu2, name_surname, keyfile);

              a_name=(char*)strtok(name_surname, ".");
              a_surname=(char*)strtok(NULL, " ");

              if(strncmp(line,"connect", strlen("connect"))==0){
                //sscanf(line,"%s %s",menu2, message_chat);
                //Verificar se está na lista, usar udp_socket com retorno do elemento
                // Verificar se já está ligado a alguém
                if((p_element=CheckInList(ptr_to_first, a_name, a_surname))!=NULL){

                    printf("Entrou no connect!\n");
                    struct in_addr ipaddress_connect;
                    inet_pton(AF_INET,p_element->ip,&(ipaddress_connect.s_addr));
                    if((tcp_connect(ipaddress_connect, p_element->port,keyfile, a_name,a_surname)==-1)){
                      printf("Authentication failed! Closing connection...\n");
                    }
                    printf("Saiu do connect!\n");

                }else{printf("Element not found on cache. Please do a find first.\n");}
              }else if(strcmp(menu2,"find")==0){    //-----Find----//

                  printf("Find [NAME.SURNAME]:   %s.%s\n", a_name,a_surname);
                  sprintf(message,"QRY %s.%s",a_name,a_surname);
                  if ((p_element = udp_socket(ipaddress_snp, porto_snp_udp,&message, 1))!=NULL){
                    printf("Retornou elemento!\n");
                    ptr_to_first = addElement(ptr_to_first, *p_element, num_elements_ptr);
                  }else{
                  printf("Retornou NULL");
                  }

            }else if(strcmp(menu2,"print")==0){    //-----Find----/
                printList(ptr_to_first);

              }else{

              printf("Wrong command...\n");
              }

            }
        }

        if(FD_ISSET(fd_tcp,&readset)){ //Socket tcp
            printf("Activity on socket fd_tcp...\n");
          if((newfd=accept(fd_tcp,(struct sockaddr*)&addr,&addrlen))==-1)exit(1);//error
            printf("Connection accepted!\n");
            switch(state)
            {
              case idle: afd = newfd; state=busy; printf("Server was in idle state  msg_rcv = [%d]   afd [%d]      fd_tcp [%d]\n", msg_rcv_f, afd, fd_tcp);
               break;
              case busy: ptr=strcpy(buffer,"Busy!\n");
                        nbytes=6;
                        nleft=nbytes;
                        while(nleft>0){
                            nwritten=write(newfd,ptr,nleft);
                            if(nwritten<=0)exit(1);//error
                            nleft-=nwritten;
                            ptr+=nwritten;
                        }
                        close(newfd);
                        printf("Server is in busy state\n");
                        break;
            }
        }


        if((state==busy) && (FD_ISSET(afd,&readset)))   {
            printf("--Afd activated... msg_rcv_f = %d \n", msg_rcv_f);
           if(auth_f==0){
            keyfile_p = read_tcp(afd);
            printf("Keyfile received from NAME: [%s]\n", keyfile_p);
            if((auth_success=authentication_client(addr,keyfile_p,afd))==-1){
              printf("Authentication failed! Closing connection...\n");
              close(afd);
              state=idle;
            }else{
              auth_f=1;
              printf("Authentication succeeded! You can start to chat.\n");
            }
           }else{

           if((n=read(afd,buffer,128))!=0){
              printf("--Reading from socket...\n");
              if(n==-1)exit(1);//error
                write(1,"Message received: ", 18);
                write(1, buffer, n);
                write(1,"\n",1);
           }
           printf("--Exiting afd sokcet if...\n");
           //connection closed by peer
           }

        }
    }
    //Fre
    free(message);

    exit(0);
}








