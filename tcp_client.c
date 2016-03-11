#include "tcp_client.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <arpa/inet.h>
#include <unistd.h>

/*int authentication_server(struct sockaddr_in addr, char keyfile[], int afd, char * ptr3){
  char * ptr, challenge[10], buffer[10], answer_s[4]; int nleft, nbytes;
  time_t t;
  srand((unsigned) time(&t));
  int rand_num=0, answer=0, i=1;
  FILE *fp_in;

  if( (fp_in = fopen(keyfile, "r"))==NULL){
      printf("\n FILE NOT FOUND \n");
      exit(-1);
  }

  rand_num=rand() % 255;
  sprintf(challenge, "AUTH %d", rand_num);

  ptr=strcpy(buffer,challenge);
  nbytes=strlen(buffer);
  nleft=nbytes;
  while(nleft>0)  {
    nwritten=write(fd_tcp,ptr,nleft);
    if(nwritten<=0)exit(1);//error
    nleft-=nwritten;
    ptr+=nwritten;
  }

  while (fgets(answer_s,4, fp_in)){
      if(i==rand_num){
        sscanf(answer_s,"%d", answer);
        printf("The shall be: %d", answer);
        break;
      }
      i++;
  }
}*/

int authentication_client(struct sockaddr_in addr, char *keyfile, int fd){
  char answer_s[9], answer_read[9], random_line_s[9], challenge_s[9], str[9];
  int random_line;
  long nread, nwritten;

  time_t t;
    srand((unsigned) time(&t));
    int answer=0, i=1, answer_rcv=-1, challenge_rcv=-1;
    FILE *fp_in;
    printf("Keyfile: %s\n",keyfile);
    if( (fp_in = fopen(keyfile, "r"))==NULL){
        printf("\n FILE NOT FOUND \n");
        exit(-1);
    }

    random_line=(rand() % 255)+1;
  i=1;
  while (fgets(answer_s,6, fp_in)){
      sscanf(answer_s,"%d", &answer);
      if(i==random_line){
        printf("The answer to random line %d is: %d\n", random_line, answer);
        break;
      }
      i++;
  }

  /*ptr=strcpy(buffer,random_line_s);
  nbytes=strlen(ptr3);
  nleft=nbytes;
  while(nleft>0)  {
    nwritten=write(fd_tcp,ptr,nleft);
    if(nwritten<=0)exit(1);//error
    nleft-=nwritten;
    ptr+=nwritten;
  }*/

  // Writes challenge to other peer
  sprintf(random_line_s, "AUTH %d", random_line);
  nwritten=write(fd,random_line_s,9); if(nwritten<=0)exit(1);//error
  printf("Challenge sent: %s with correct answer %s\n", random_line_s, answer_s);

  //Reads answer to the challenge
  nread=read(fd,answer_read,9); if(nread==-1)exit(1);
  sscanf(answer_read,"AUTH %d",&answer_rcv) ;
  printf("Answer received: %s [number: %d]\n", answer_read, answer_rcv);

  if(answer_rcv == answer){
    printf("Peer answered correctly!\n ");

  }else{
  return -1;
  }
  nread=0;

  //Wait and listen to the challenge by other peer
  //while(nread==0){
    nread=read(fd,challenge_s,9); if(nread==-1)exit(1);
    sscanf(challenge_s,"AUTH %d",&challenge_rcv);
    printf("Challenge received: %s number: [%d]\n", challenge_s, challenge_rcv);
  //}

  i=1;
  printf("keyfile: %s", keyfile);
  fclose(fp_in);

  if( (fp_in = fopen(keyfile, "r"))==NULL){
        printf("\n FILE NOT FOUND \n");
        exit(-1);
  }
  while (fgets(answer_s,6, fp_in)){
      printf("%d:    %s", i, answer_s);
      sscanf(answer_s,"%d", &answer);
      if(i==challenge_rcv){
        printf("\nThe answer to the challenge %d is %d\n", challenge_rcv,answer);
        break;
      }
      i++;

  }

  // Answer back
  sprintf(str,"AUTH %d", answer);
  nwritten=write(fd,str,9); if(nwritten<=0)exit(1);//error
  printf("Last response to connection sent : %s\n", str);
  return 1;
}

void write_tcp(char * ptr3, int fd){
  char buffer[128];
  long nbytes, nleft, nwritten;
  char *ptr;
  ptr=strcpy(buffer,ptr3);
  nbytes=strlen(ptr3);
  nleft=nbytes;
  while(nleft>0)  {
    nwritten=write(fd,ptr,nleft);
    if(nwritten<=0)exit(1);//error
    nleft-=nwritten;
    ptr+=nwritten;
  }
}


char * read_tcp(int fd){

  char buffer[128], command[32]; int i=0; long nread;
  char *keyfile = (char*)malloc(64);

  /*while(i!=-2)  {
    nread=read(fd,ptr,128);
    if(nread==-1)exit(1);//error
    else if(nread==0)break; //closed by peer
    i=0;
    while(i<nread){
      if (ptr[i]=='\n'){
        i=-2;
        break;
      }
      buffer[j]=ptr[i];
      j++;i++;
    }
  }*/

  while(i!=-2)  {
    nread=read(fd,buffer,128);

    printf("Nread: %lu | Buffer: %s\n", nread, buffer);;
    if(nread==-1)exit(1);//error
    else if(nread==0)break; //closed by peer
    i=0;
    while(i<nread){
      printf("Cycling... buff[%d] = %c\n", i, buffer[i]);
      if (buffer[i]== '\n'){
        printf("\n found on buffer! Buffer: %s\n", buffer);
        i=-2;
        break;
      }
      i++;
    }
  }

   if (strncmp(buffer, "NAME", 4)==0){
    sscanf(buffer, "%s %s",command, keyfile);
   }
   printf("Inside read_tcp()... Command: %s | Keyfile: %s\n", command, keyfile);

  return keyfile;
}

int tcp_connect(struct in_addr ip,int port, char keyfile[], char * a_name, char * a_surname) {
  fd_set readset;
  int i=0,msg_rcv_f=0, answer=-1, answer_rcv=-1,challenge_rcv=-1, random_line=-1,fd_tcp, disconnect=0, fd_file, result;
  long nread, nwritten, nbytes, nleft, n;
  char *ptr, buffer[128], answer_s[9], challenge_s[9], random_line_s[9], line[128], menu[128], str[9];
  char * ptr3 = (char*)malloc(126*sizeof(char));
  struct sockaddr_in addr;
  /* ... */
  time_t t;
  srand((unsigned) time(&t));
  fd_tcp=socket(AF_INET,SOCK_STREAM,0);//TCP socket
  if(fd_tcp==-1)exit(1);//error

  memset((void*)&addr,(int)'\0',sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr=ip;
  addr.sin_port=htons(port);

  n=connect(fd_tcp,(struct sockaddr*)&addr,sizeof(addr));



  if(n==-1){perror("Error: ");exit(1);}//error
  /* ... */
  printf("Entrou no tcp_client!\n");

  // Cliente dá-se a conhecer à outra parte
  sprintf(ptr3, "NAME %s.%s\n", a_name, a_surname);
  write_tcp(&ptr3[0], fd_tcp);
  printf("Name sent to peer: %s\n", ptr3);

  //Reads challenge
  nread=read(fd_tcp,challenge_s,9); if(nread==-1)exit(1);
  sscanf(challenge_s,"AUTH %d",&challenge_rcv);
  printf("Challenge received: %s It will check on file %s for line %d\n", challenge_s, keyfile, challenge_rcv);

  FILE *fp_in;
  printf("Keyfile: %s\n",keyfile);
    if( (fp_in = fopen(keyfile, "r"))==NULL){
        printf("\n FILE NOT FOUND \n");
        exit(-1);
    }

  i=1;
  while (fgets(answer_s,6, fp_in)){
      sscanf(answer_s,"%d", &answer);
      if(i==challenge_rcv){
        printf("The answer to the challenge %d is %d\n", challenge_rcv,answer);
        break;
      }
      i++;
  }

  sprintf(str,"AUTH %d", answer);
  nwritten=write(fd_tcp,str,9); if(nwritten<=0)exit(1);//error
  printf("Answer sent: %s\n", answer_s);

  //Challenge to peer
    random_line=(rand() % 255)+1;
    sprintf(random_line_s, "AUTH %d", random_line);
  fclose(fp_in);

  if( (fp_in = fopen(keyfile, "r"))==NULL){
    printf("\n FILE NOT FOUND \n");
    exit(-1);
  }
  i=1;
  while (fgets(answer_s,6, fp_in)){
      sscanf(answer_s,"%d", &answer);
      if(i==random_line){
        printf("The answer to line %d is: %d\n",random_line, answer);
        break;
      }
      i++;
  }

  // Writes challenge to other peer
  nwritten=write(fd_tcp,random_line_s,9); if(nwritten<=0)exit(1);//error
  printf("Challenge sent: %s\n", random_line_s);

  //Reads answer to the challenge
  nread=read(fd_tcp,answer_s,9); if(nread==-1)exit(1);
  printf("Answer received: %s\n", answer_s);
  sscanf(answer_s,"AUTH %d",&answer_rcv);

  if(answer_rcv == answer){
    printf("Peer answered correctly!\n ");
  }else{
    close(fd_tcp);
    return -1;}

  //if((n=read(fd_tcp,buffer,128))!=0){
    //if(n==-1)exit(1);//error

  while(disconnect!=1){

    fd_file = fileno(stdin);
    FD_ZERO(&readset);
    FD_SET(fd_tcp,&readset);
    FD_SET(fd_file, &readset);

    result = select(fd_file+fd_tcp+1,&readset,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
    if(result==-1)exit(1);//error

    if(FD_ISSET(fd_file,&readset)){
        printf("Activity on keyboard\n");
        fgets(line,128,stdin);
        sscanf(line,"%s",menu);

        if(strncmp(menu, "disconnect", strlen("disconnect"))==0){
          printf("Disconnecting...\n");
          disconnect=1;
        } else if(strncmp(menu, "message", strlen("message"))==0) {
            ptr3=(char*)strtok(line, " ");
            ptr3=(char*)strtok(NULL, "\0");
            msg_rcv_f=1;

            if (msg_rcv_f==1){

            ptr=strcpy(buffer,ptr3);
            nbytes=strlen(ptr3);
            nleft=nbytes;
            printf("-----writing message to other party...\n");
            while(nleft>0){
              printf("...");
              nwritten=write(fd_tcp,ptr,nleft);
              if(nwritten<=0)exit(1);//error
              nleft-=nwritten;
              ptr+=nwritten;
            }
            printf("\n");
            nleft=nbytes; ptr=buffer;
            msg_rcv_f=0;
          }

        }else{
          printf("Wrong command. Neither message or disconnect\n");
        }
    }

        if(FD_ISSET(fd_tcp,&readset)){
          if((n=read(fd_tcp,buffer,128))!=0){
              printf("-----Reading from socket...\n");
              if(n==-1)exit(1);//error
                write(1,"Message received: ", 18);
                write(1, buffer, n);
                write(1,"\n",1);
           }

          printf("-----Exiting socket...\n");
        }
  }
  //}
          return 1;
}
