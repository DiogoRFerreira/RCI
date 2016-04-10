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
#include <stdint.h>
#include <inttypes.h>
#define timeout 3
#define max(A,B) ((A)>=(B)?(A):(B))

// Function that is called when a user tries to connect to the schat that calls this function.
// It handles all authentication process. Returns 1 for success and -1 for failure.
int authentication_client(struct sockaddr_in addr, char *keyfile, int fd){
  char answer_s[6], answer_read[6], random_line_s[6], challenge_s[6], str[6];
  int random_line, answer=0;
  unsigned char answer_rcv, challenge_rcv;
  long nread, nwritten;

  time_t t;
    srand((unsigned) time(&t));
    int i=1;
    FILE *fp_in;

    printf("--------------------------------------Starting Authentication-------------------------------------------------\n");
    if( (fp_in = fopen(keyfile, "r"))==NULL){
        printf("Error: Authentication file not found! \n");
        return -1;
    }
  random_line=(rand() % 255);
  i=1;
  while (fgets(answer_s,6, fp_in)){
      sscanf(answer_s,"%d", &answer);
      if(i==(random_line+1)){
        break;
      }
      i++;
  }
  // Writes challenge to other peer
  sprintf(random_line_s, "AUTH %c", (unsigned char)random_line);
  nwritten=write(fd,random_line_s,6);
  if(nwritten<=0){perror("Error in write: ");exit(7);}
  //printf("Challenge sent: %d with correct answer %d\n", (random_line+1), answer);

  //Reads answer to the challenge
  nread=read(fd,answer_read,6); if(nread==-1){perror("Error in read: ");exit(9);}
  sscanf(answer_read,"AUTH %c",&answer_rcv);
  //printf("Answer received: %hhu \n", answer_rcv);
  if(answer_rcv == answer){
    printf("Peer answered correctly to 1st step.\n");
  }else{
  return -1;
  }
  nread=0;

  //Listen to the challenge by other peer
	nread=read(fd,challenge_s,6); if(nread==-1){perror("Error in read: ");exit(9);}
	sscanf(challenge_s,"AUTH %c",&challenge_rcv);
	//printf("Challenge received: %hhu\n", challenge_rcv+1);

  i=1; fclose(fp_in);

  if( (fp_in = fopen(keyfile, "r"))==NULL){
        printf("Error: Authentication file not found! \n");
        return -1;
  }
  while (fgets(answer_s,6, fp_in)){
      sscanf(answer_s,"%d", &answer);
      if(i==(challenge_rcv+1)){
        break;
      }
      i++;
  }

  // Answer back
  sprintf(str,"AUTH %c", (unsigned char)answer);
  nwritten=write(fd,str,6); if(nwritten<=0){perror("Error in write: ");exit(7);}
  //printf("Answer sent : %hhu\n", answer);
  printf("--------------------------------------------------------------------------------------------------------------\n");
  return 1;
}
// Utility function that writes a string pointed by ptr3 into tcp socket
void write_tcp(char * ptr3, int fd){
  char buffer[128];
  long nbytes, nleft, nwritten;
  char *ptr;
  ptr=strcpy(buffer,ptr3);
  nbytes=strlen(ptr3);
  nleft=nbytes;
  while(nleft>0)  {
    nwritten=write(fd,ptr,nleft);
    if(nwritten<=0){perror("Error in write: ");exit(7);}
    nleft-=nwritten;
    ptr+=nwritten;
  }
}

// Utility function that reads from a tcp socket and returns the keyfile read
char * read_tcp(int fd){
  char buffer[128]; int i=0; long nread;
  char *buff = (char*)malloc(128*sizeof(char));
  while(i!=-2)  {
    nread=read(fd,buffer,128);
    if(nread==-1){perror("Error in read: ");exit(9);//error
    }else if(nread==0)break; //closed by peer
    i=0;
    while(i<nread){
      if (buffer[i]== '\n'){
        i=-2;
        break;
      }
      i++;
    }
  }
	strcpy(buff,buffer);
  return buff;
}

// Handles all instructions of the schat tcp client side.
int tcp_connect(struct in_addr ip,int port, char * keyfile, char * a_name, char * a_surname, char * dest_name, char*dest_surname) {
  fd_set readset;
  int i=0,fd_tcp, disconnect=0, fd_file, result;
  int random_line=-1, answer=-1;
  unsigned char answer_rcv=-1,challenge_rcv=-1;
  long nread, nwritten, nbytes, nleft, n;
  char *ptr, buffer[128], answer_s[6], challenge_s[6], random_line_s[6], line[128], str[6];
  char * ptr3 = (char*)malloc(126*sizeof(char)), name_display[32];
  struct sockaddr_in addr;

  struct timeval tv;
	tv.tv_sec = timeout;//Timeout time secs
	tv.tv_usec = 0;
  FILE *fp_in;

  printf("--------------------------------------Starting Authentication-------------------------------------------------\n");
  if( (fp_in = fopen(keyfile, "r"))==NULL){
    printf("Error: Authentication file not found! \n");
    return -1;
  }

  time_t t;
  srand((unsigned) time(&t));
  fd_tcp=socket(AF_INET,SOCK_STREAM,0);//TCP socket
  if(fd_tcp==-1)exit(1);//error
setsockopt(fd_tcp, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));
  memset((void*)&addr,(int)'\0',sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr=ip;
  addr.sin_port=htons(port);

  n=connect(fd_tcp,(struct sockaddr*)&addr,sizeof(addr));

  if(n==-1){perror("Error in connect: ");exit(8);}//error

  // Cliente dá-se a conhecer à outra parte
  sprintf(ptr3, "NAME %s.%s %s\n", a_name, a_surname, keyfile);
  //printf("Protocol msg sent (last arg, is the keyfile entered): %s", ptr3);
  write_tcp(&ptr3[0], fd_tcp);

  //Reads challenge
  nread=read(fd_tcp,challenge_s,6); if(nread==-1){perror("Error in read: ");exit(9);}
  sscanf(challenge_s,"AUTH %c",&challenge_rcv);
  //printf("Challenge received: %hhu\n", challenge_rcv+1);
  i=1;
  while (fgets(answer_s,6, fp_in)){
      sscanf(answer_s,"%d", &answer);
      if(i==(challenge_rcv+1)){
        break;
      }
      i++;
  }
  sprintf(str,"AUTH %c", answer);
  nwritten=write(fd_tcp,str,6); if(nwritten<=0){perror("Error in write: ");exit(7);}
  //printf("Answer sent: %hhu\n", answer);

  //Challenge to peer
  random_line=(rand() % 255);

  fclose(fp_in);

  if( (fp_in = fopen(keyfile, "r"))==NULL){
	printf("Error: Authentication file not found. \n");
	return -1;
  }
  i=1;
  while (fgets(answer_s,6, fp_in)){
      sscanf(answer_s,"%d", &answer);
      if(i==(random_line+1)){
        break;
      }
      i++;
  }
  fclose(fp_in);
  // Writes challenge to other peer
  sprintf(random_line_s, "AUTH %c", random_line);
  nwritten=write(fd_tcp,random_line_s,6); if(nwritten<=0){perror("Error in write: ");exit(7);}
  //printf("Challenge sent: %hhu with correct answer %d\n", random_line+1, answer);

  //Reads answer to the challenge
  nread=read(fd_tcp,answer_s,6); if(nread==-1){perror("Error in read: ");exit(9);}
  sscanf(answer_s,"AUTH %c",&answer_rcv);
  //printf("Answer received: %hhu\n", answer_rcv);
	//Evaluate if authentication succeded
  if(answer_rcv == answer){
    printf("Peer answered correctly!\n ");
  }else{
    close(fd_tcp);
    return -1;}
	printf("--------------------------------------------------------------------------------------------------------------\n");
	// Main cycle
  while(disconnect!=1){
	//Initilize file descriptors
    fd_file = fileno(stdin);
    FD_ZERO(&readset);
    FD_SET(fd_tcp,&readset);
    FD_SET(fd_file, &readset);
	//Look for activity on file descriptors
    result = select(max(fd_file,fd_tcp)+1,&readset,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
    if(result==-1){perror("Error in select: ");exit(3);}
	// If activity is detected on keyboard as input
    if(FD_ISSET(fd_file,&readset)){
        fgets(line,128,stdin);
        if(strncmp(line, "disconnect", strlen("disconnect"))==0){
          printf("Disconnecting...\n");
          disconnect=1;
        } else if(strncmp(line, "message ", strlen("message "))==0) {
            ptr3=(char*)strtok(line, " ");
            ptr3=(char*)strtok(NULL, "\0");
            //Sends message
			ptr=strcpy(buffer,ptr3);
			nbytes=strlen(ptr3);
			nleft=nbytes;
			while(nleft>0){
			  nwritten=write(fd_tcp,ptr,nleft);
			  if(nwritten<=0){perror("Error in write: ");exit(7);}
			  nleft-=nwritten;
			  ptr+=nwritten;
			}

			nleft=nbytes; ptr=buffer;
        }else{
          printf("Wrong command. Neither message or disconnect.\n");
        }
    }
		// If a message is received
        if(FD_ISSET(fd_tcp,&readset)){
          if((n=read(fd_tcp,buffer,128))!=0){
              if(n==-1){perror("Error in read: ");exit(9);}
               sprintf(name_display, "%s %s: ",dest_name, dest_surname);
				write(1,name_display, strlen(name_display));
                write(1, buffer, n);
           }else{
              close(fd_tcp);
              disconnect=1;
              }
        }
  }
		close(fd_tcp);
          return 1;
}
