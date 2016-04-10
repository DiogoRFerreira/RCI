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
#include <signal.h>
#define timeout 3
#define max(A,B) ((A)>=(B)?(A):(B))
//	Error code (system calls):
//				-1:	Bad Arguments;
//				 1: Socket
//				 2: Bind
//				 3: Select
//				 4: Listen
//				 5: Signal
//				 6: Accept
//				 7: Write
//				 8: Connect
//				 9: Read

int main(int argc, const char * argv[]) {
	//Integers to check the arguments
    int i, j,bad_arguments=0,contagem=0;
    //Port variables
    int porto_schat_tcp=0, porto_snp_udp = 0;
	//IP variables, Surname, Message
    char * ip_snp = (char*)malloc(20*sizeof(char));
    char * ip_schat = (char*)malloc(20*sizeof(char));
    char * name_surname = (char*)malloc(64*sizeof(char));
    char * keyfile = (char*)malloc(64*sizeof(char));
    char * name=NULL,*surname=NULL, user_input[64];
	//Addresses
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
                if(inet_pton(AF_INET,argv[i+1],&(ipaddress_snp.s_addr))==1){        //Check if the ip is valid
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
                if(inet_pton(AF_INET,argv[i+1],&(ipaddress.s_addr))==1){//Check if the ip is valid
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

    //Missing arguments , too many arguments or bad arguments
    if((argc!=11 || bad_arguments == 1)){
        printf("Invocar aplicação da seguinte forma: ./schat -n name.surname -i ip -p scport -s snpip -q snpport\n");
        exit(-1);
    }

    //Print of the values
    printf("SCHAT:\nName: %s\nIP address: %s\nPort: %d\n\n",surname,ip_schat,porto_schat_tcp);
    printf("SNP:\nIP address: %s\nPort: %d\n\n",/*inet_ntoa(ipaddress_snp)*/ip_snp,porto_snp_udp);

    //---------Socket Server e o Menu------//
    //SIGPIPE signal
    void (*old_handler)(int);//interrupt handler
    if((old_handler=signal(SIGPIPE,SIG_IGN))==SIG_ERR)exit(5);//error

    fd_set readset;
    struct sockaddr_in addr;
    socklen_t addrlen;
    int exit_menu=0,fd_file, fd_tcp, afd=0, msg_rcv_f=0, newfd; long nbytes, nleft, nwritten, n;
    char menu2[64], line[128], command[128], *a_name, *a_surname,*display_name, *display_surname, *ptr, buffer[128], *ptr3, *keyfile_p=(char*)malloc(64*sizeof(char)), name_display[32];
     char *buff = (char*)malloc(128);
    enum {idle,busy} state;
    element * p_element=NULL, *ptr_to_first=NULL;
    int num_elements=0, maxfd=0;
    int *num_elements_ptr= &num_elements;
    //int prev_num_elements=*num_elements_ptr;
    int auth_f=0, auth_success=0;
    //prev_num_elements=0;

    char * message = (char*)malloc(126*sizeof(char));

    //TCP Server bind, listen
    //TCP socket
    if((fd_tcp=socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("Socket: "); exit(1);}
    memset((void*)&addr,(int)'\0',sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY);//Any address can connect
    addr.sin_port=htons(porto_schat_tcp);//Tcp schat port
    //Bind
    if(bind(fd_tcp,(struct sockaddr*)&addr,sizeof(addr))==-1) {
        perror("Bind failed: "); exit(2); }
    //Listen
    if(listen(fd_tcp,5)==-1){
		perror("Listen failed: "); exit(4);} //error
    state = idle;
    addrlen=sizeof(addr);
	printf("Welcome to SCHAT interface. Commands are: {join, leave, exit}\n");
    //Main cycle
    while(exit_menu==0){
		//Initilize file descriptors
        fd_file = fileno(stdin);
        FD_ZERO(&readset);
        FD_SET(fd_tcp,&readset);
        if(state==busy){FD_SET(afd,&readset);}
        FD_SET(fd_file, &readset);
        //Depending on state, look for activity on file descriptors
        if(state==busy){
			maxfd = max(fd_file,fd_tcp); 
			if(select(max(maxfd,afd)+1,&readset,NULL,NULL,NULL)==-1) {
				perror("Select failed: "); exit(3);}
        }else{
			if(select(max(fd_file,fd_tcp)+1,&readset,NULL,NULL,NULL)==-1){
				perror("Select failed: "); exit(3);}
			}
		// If activity is detected on keyboard as input
        if(FD_ISSET(fd_file,&readset)){
            fgets(line,128,stdin);
            if((strncmp(line,"join",strlen("join"))==0)){//-------Join-------//
                sprintf(message,"REG %s.%s;%s;%d",name,surname, ip_schat,porto_schat_tcp);
                udp_socket(ipaddress_snp, porto_snp_udp,&message, 0);
                printf("Protocol msg sent: %s\n", message);
            }else if((strncmp(line,"leave", strlen("leave"))==0)){//------Leave-------//
                sprintf(message,"UNR %s.%s",name,surname);
                udp_socket(ipaddress_snp, porto_snp_udp,&message,0);
                printf("Protocol msg sent: %s\n", message);
            }else if(strncmp(line,"exit",4)==0){//------Exit-------//
                printf("Exiting SCHAT...\n");
                sprintf(message,"UNR %s.%s",name,surname);
                udp_socket(ipaddress_snp, porto_snp_udp,&message,0);
                close(fd_tcp);
                exit_menu=1;
            }else if(strncmp(line,"disconnect",strlen("disconnect"))==0){//------Disconnect-------//
                if(state==busy) {
                    close(afd);
                    state=idle;
                }else{
                    printf("No schat to disconnect from. (Not connected to any schat)\n");
                }
            }else if(strncmp(line,"message ",strlen("message "))==0){
                if (state == busy){
                    ptr3=(char*)strtok(line, " ");
                    ptr3=(char*)strtok(NULL, "\0");
                    msg_rcv_f = 1;
                    if (msg_rcv_f==1){
                      ptr=strcpy(buffer,ptr3); nbytes=strlen(ptr3); nleft=nbytes;
                      //Write message 
                      while(nleft>0){
                        nwritten=write(afd,ptr,nleft);
                        if(nwritten<=0)exit(1);//error
                        nleft-=nwritten;  ptr+=nwritten;
                      }
                      nleft=nbytes; ptr=buffer;
                      msg_rcv_f=0;
                  }
                }else {
                    printf("Not connected to any schat.\n");
                }
            }else {
              int num_vars_read;
              num_vars_read = sscanf(line,"%s %s %s",menu2, name_surname, keyfile);

              a_name=(char*)strtok(name_surname, ".");
              a_surname=(char*)strtok(NULL, " ");

              if((strncmp(menu2,"connect", strlen("connect"))==0) && (num_vars_read==3)){
                //Verificar se está na lista, usar udp_socket com retorno do elemento
                // Verificar se já está ligado a alguém
                if((p_element=CheckInList(ptr_to_first, a_name, a_surname))!=NULL){
                    
                    struct in_addr ipaddress_connect;
                    inet_pton(AF_INET,p_element->ip,&(ipaddress_connect.s_addr));
                    if((tcp_connect(ipaddress_connect, p_element->port,keyfile, name,surname, a_name, a_surname)==-1)){
                      printf("Authentication failed. Closing connection...\n");
                    }
                }else{printf("Element not found on cache. Please do a find first.\n");}
              }else if((num_vars_read>1) && (a_surname!=NULL) && (((strncmp(menu2,"find", 4)==0) && (a_name[0]>='A') &&
                            (a_name[0]<='z')) && (a_surname[0]>='A') && (a_surname[0]<='z'))){    //-----Find----//
                  //Check if name and surname initiate with capital letters. If so, try to find it
                  sprintf(message,"QRY %s.%s",a_name,a_surname);
                  // If it receives valid element but it already is in the list, update it.
					if ((p_element = udp_socket(ipaddress_snp, porto_snp_udp,&message,1))!=NULL){ 
						if(CheckInList(ptr_to_first, p_element->name, p_element->surname)!=NULL){
							printf("Name.Surname already in local list, deleting old one.\n");
							ptr_to_first=deleteElement(ptr_to_first, p_element->name, p_element->surname, num_elements_ptr);
						}
					 	ptr_to_first = addElement(ptr_to_first, p_element, num_elements_ptr);
                    }

            }else if(strcmp(menu2,"print")==0){    //-----Print list in schat----/
                printList(ptr_to_first);
              }else{
				printf("Command not found.\n");
              }
            }
        }
        //	Activity on socket which receives incoming requests
        if(FD_ISSET(fd_tcp,&readset)){ //Socket tcp
          if((newfd=accept(fd_tcp,(struct sockaddr*)&addr,&addrlen))==-1)exit(6);//error
            printf("Connection accepted.\n");
            switch(state)
            {
              case idle: afd = newfd; state=busy; break;
              case busy: ptr=strcpy(buffer,"Server is busy!\n");
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
        // Busy state (chatting) and activity on chat socket
        if((state==busy) && (FD_ISSET(afd,&readset)) )   {
           //If user did not autenticate yet, do it
           if(auth_f==0){
				buff = read_tcp(afd);
				if (strncmp(buff, "NAME", 4)==0){
					sscanf(buff, "NAME %s %s",command, keyfile_p);
					display_name=(char*)strtok(command, ".");
					display_surname=(char*)strtok(NULL," ");
					sprintf(name_display, "%s %s: ",display_name, display_surname);
				}
				if((auth_success=authentication_client(addr,keyfile_p,afd))==-1){
				  printf("Authentication failed! Closing connection...\n");
				  close(afd);
				  state=idle;
				}else{
				  auth_f=1; 
				  printf("Authentication succeeded! You can start to chat.\n");
				}
           }else{
				if((n=read(afd,buffer,128))==-1){
					perror("read failed: ");exit(7);//error
				}
				if(n!=0){
					write(1,name_display, strlen(name_display));
					write(1, buffer, n);

				}else{
					printf("Other user disconnected.\n");
					close(afd);
					state=idle;
					auth_f=0;
				}
			}
		}
    }
    //Free
    free(message); free(ip_snp); free(ip_schat); free(name_surname); free(keyfile);
    exit(0);
}








