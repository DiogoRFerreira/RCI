#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include "struct_no.h"
#include <math.h>
#include "teste1.h"

extern int errno;

// funcao que devolve o IP da maquina
char * getip(char * cmd1){
    FILE *f;
    char line[100] , *p , *c;
     
    f = fopen("/proc/net/route" , "r");
     
    while(fgets(line , 100 , f))
    {
        p = strtok(line , " \t");
        c = strtok(NULL , " \t");
         
        if(p!=NULL && c!=NULL)
        {
            if(strcmp(c , "00000000") == 0)
            {
                break;
            }
        }
    }
 
    int fm = AF_INET;
    struct ifaddrs *ifaddr, *ifa;
    int family , s;
    char host[NI_MAXHOST];
 
    if (getifaddrs(&ifaddr) == -1) 
    {
        printf("Erro: %s\n", strerror(errno));
        exit(0);
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) 
    {
        if (ifa->ifa_addr == NULL)
        {
            continue;
        }
 
        family = ifa->ifa_addr->sa_family;
 
        if(strcmp( ifa->ifa_name , p) == 0)
        {
            if (family == fm) 
            {
                s = getnameinfo( ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6) , host , NI_MAXHOST , NULL , 0 , NI_NUMERICHOST);
                 
                if (s != 0) 
                {
                    printf("Erro: %s\n", strerror(errno));
                    exit(0);
                }
            }
        }
    }
	strcpy(cmd1, host);
    freeifaddrs(ifaddr);
		
    return cmd1;
}
	
// funcao que devolve o IP da maquina tejo
char * get_tejo_ip(char * cmd){
	
	struct hostent *h;
	struct in_addr *a;
	if((h=gethostbyname("tejo.tecnico.ulisboa.pt"))==NULL){
		printf("Erro: %s\n", strerror(errno));
		exit(1);//error
	}
	a=(struct in_addr*)h->h_addr_list[0];
	strcpy(cmd, inet_ntoa(*a));
	return cmd;
}

//funcao responsavel pelo cliente TCP usado no programa

void tcp_client(char * cmd, char * ip2, int tcp_port, char * fd_out, struct info_no *no, int tcp_port2, char *abc_port){
	int fd, n;
	struct sockaddr_in addr, ip_addr;
	char ip[128];
	char buffer1[128];
	
	fd = socket(AF_INET,SOCK_STREAM,0);//TCP socket	
	if(fd == -1){
		printf("Erro: %s\n", strerror(errno));
		exit(1);//error
	 }
	strcpy(ip, ip2);
	inet_aton(ip, &ip_addr.sin_addr); 
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr = ip_addr.sin_addr;
	addr.sin_port = htons(tcp_port);
	
	n = connect(fd,(struct sockaddr*)&addr, sizeof(addr));
	if(n == -1){
		printf("Erro: %s\n", strerror(errno));
		 exit(1);//error
	 }
	char id[128] = "ID ";
	strcat(id, cmd);
	n = write(fd,id,strlen(id));
	bzero(buffer1, 128);//Fill every position with \0s
	n = read(fd, buffer1, 127);
	close(fd);
	printf("Resposta_Arranque: %s\n", buffer1);//stdout
	char res1[128];
	char res2[128];
	char res3[128];
	if(strncmp(buffer1, "SUCC", 4) == 0){
		int i = 0;
		char * pch;
		pch = strtok (buffer1," ");
		while (pch != NULL){
			i++;
			if(i == 2) strcpy(res1, pch);
			
			if(i == 3) strcpy(res2, pch);
			
			if(i == 4) strcpy(res3, pch);
				
			pch = strtok (NULL, " ");
		}	
		strcpy(no->id_succi, res1);
		strcpy(abc_port, res3);
	}
	if(strcmp(cmd, res1) == 0){
		printf("Escolha outro identificador para o no.\n");
		return;
	}else{
		//abre sessao tcp 
	fd = socket(AF_INET,SOCK_STREAM,0);//TCP socket	
	if(fd == -1){
		printf("Erro: %s\n", strerror(errno));
		 exit(1);//error
	 }
	strcpy(ip, res2);
	inet_aton(ip, &ip_addr.sin_addr); 
	tcp_port = atoi(res3);
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr = ip_addr.sin_addr;
	addr.sin_port = htons(tcp_port);
	n = connect(fd,(struct sockaddr*)&addr, sizeof(addr));
	
	strcpy(no->succi_IP, res2);
	strcpy(no->succi_TCP, res3);
	strcpy(no->id_predi, no->id_succi);
	strcpy(no->predi_IP, res2);
	strcpy(no->predi_TCP, res3);
	
	if(n == -1){
		printf("Erro: %s\n", strerror(errno));
		 exit(1);//error
	 }
	char new[128] = "NEW ";
	sprintf(res3, "%d", tcp_port2);
	strcat(new, cmd);
	strcat(new, " ");
	strcat(new, ip2);
	strcat(new, " ");
	strcat(new, res3);
	n = write(fd,new,strlen(new));
	bzero(buffer1, 128);
	
	no->arranque = 0;
	strcpy(no->IP, ip2);
	strcpy(no->TCP, res3);
	
	sprintf(fd_out, "%d", fd);

	return;		
	}
	
	
	

}

//funcao que junta nos COM PROCURA POR ID num anel
void reg(char * cmd, char * ip2, int boot_port, int tcp_port, int flag, struct info_no * no, char * desc, char * abc_ip, char * abc_port){
	int i, j, send, addrlen, fd;
	int rec;
	char string[128];
	char res1[128];
	char res2[128];
	char res3[128];
	char res4[128];
	char resx[128];
	char reg[128] = "REG ";
	char buffer[128];
	char host[128] = " ";
	char ip[128];
	struct sockaddr_in addr;
	struct hostent *h;
	struct in_addr *a;
	
	addrlen = sizeof(addr);
	
	fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
	if(fd==-1)exit(1);//error	
	if((h=gethostbyname(ip2))==NULL){
		printf("Erro: %s\n", strerror(errno));
		exit(1);
	}
	a=(struct in_addr*)h->h_addr_list[0];
	memset((void*)&addr,(int)'\0',sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr=*a;
	addr.sin_port=htons(boot_port);

	j = 0;
	for(i = 5; i <= strlen(cmd)-1; i++){
		string[j] = cmd[i];
		j++;
	}	
	string[j-1] = '\0';
	j = 0;
	for(i = 0; i < strlen(string); i++){
		if((string[i] != ' ')){
			res1[i] = string[i];
		}else{
			res1[i] = '\0';
			break;
		}
	}
	i++;
	while(i <= strlen(string)){
		if((string[i] != ' ') && (string[i] != '\0')){
			res2[j] = string[i];
			j++;
		}else{
			res2[j] = '\0';
			break;
		}
		i++;
	}

	char query[128] = "BQRY ";
	strcat(query, res1);
	strcat(query, " ");
	strcat(query, res2);
	strcpy(resx, res2);
	strcpy(cmd, query);
	
	strcpy(ip, getip(host));
	addrlen=sizeof(addr);
		
	send=sendto(fd,cmd,strlen(cmd)-1,0, (struct sockaddr*)&addr, sizeof(addr));		
	if(send==-1){
		printf("Erro: %s\n", strerror(errno));
		exit(1);//error
	}
	rec = recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr, &addrlen);
	if(rec==-1){
		printf("Erro: %s\n", strerror(errno));
		exit(1);//error
	}
	if(strncmp(buffer, "NOK", 3) == 0){
		printf("Unknown command.\n");
	}else{
		write(1,"Resposta_UDP: ",14);//stdout
		write(1,buffer,rec);
		printf("\n");
	}
	char tcp_porto[128];
	sprintf(tcp_porto, "%d", tcp_port);
	if(strncmp(buffer, "EMPTY", 5) == 0){
		strcat(reg, res1);
		strcat(reg, " ");
		strcat(reg, res2);
		strcat(reg, " ");
		strcat(reg, ip);
		strcat(reg, " ");
		strcat(reg, tcp_porto);
		strcpy(res2, " ");
		send = sendto(fd,reg,strlen(reg),0, (struct sockaddr*)&addr, sizeof(addr));		
		if(send == -1){
			printf("Erro: %s\n", strerror(errno));
			exit(1);//error
		}
		rec = recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr, &addrlen);
		if(rec == -1){
			printf("Erro: %s\n", strerror(errno));
			exit(1);//error
		}
		if(strncmp(buffer, "NOK", 3) == 0){
			printf("Unknown command.\n");
		}else{
			flag = -1;
			strcpy(no->anel, res1);
		    no->arranque = 1;
		    strcpy(no->id, resx);
		    strcpy(no->IP, ip);
		    strcpy(no->TCP, tcp_porto);
		    strcpy(no->id_succi, resx);
		    strcpy(no->succi_IP, ip);
		    strcpy(no->succi_TCP, tcp_porto);
		    strcpy(no->id_predi, resx);
		    strcpy(no->predi_IP, ip);
		    strcpy(no->predi_TCP, tcp_porto);
			write(1,"Resposta_UDP: ",14);//stdout
			write(1,buffer,rec);
			printf("\n");
		}		
	}else{	
			j = 0;
			for(i = 5; i <= strlen(buffer)-1; i++){
				string[j] = buffer[i];
				j++;
			}	
			string[j] = '\0';
			j = 0;
			for(i = 0; i < strlen(string); i++){
				if((string[i] != ' ')){
					res1[i] = string[i];
				}else{
					res1[i] = '\0';
					break;
				}
			}
			i++;
			while(i <= strlen(string)){
				if((string[i] != ' ') && (string[i] != '\0')){
					res2[j] = string[i];
					j++;
				}else{
					res2[j] = '\0';
					break;
				}
				i++;
			}
			j = 0;
			i++;
			while(i <= strlen(string)){
				if((string[i] != ' ') && (string[i] != '\0')){
					res3[j] = string[i];
					j++;
				}else{
					res3[j] = '\0';
					break;
				}
				i++;
			}
			j = 0;
			i++;
			while(i <= strlen(string)){
				if((string[i] != ' ') && (string[i] != '\0')){
					res4[j] = string[i];
					j++;
				}else{
					res4[j] = '\0';
					break;
				}
				i++;
			}
			strcpy(no->anel, res1);
			strcpy(abc_ip, res3);
			strcpy(abc_port, res4);
			strcpy(no->id, resx);
			tcp_client(resx, res3, atoi(res4), desc, no, tcp_port, abc_port);
			
		}	
}

//funcao que junta nos num anel SEM PROCURA POR ID	
void reg_succi(char * anel, char * no_x, char * suc, char * suc_ip, char * suc_tcp, struct info_no * no, int myport, char * desc){
	int fd, n;
	char ip[128];
	int tcp_port;
	struct sockaddr_in addr, ip_addr;
	
	fd = socket(AF_INET,SOCK_STREAM,0);//TCP socket	
	if(fd == -1){
		printf("Erro: %s\n", strerror(errno));
		 exit(1);//error
	 }
	strcpy(ip, suc_ip);
	inet_aton(ip, &ip_addr.sin_addr); 
	tcp_port = atoi(suc_tcp);
	
	memset((void*)&addr, (int)'\0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr = ip_addr.sin_addr;
	addr.sin_port = htons(tcp_port);
	n = connect(fd,(struct sockaddr*)&addr, sizeof(addr));
	if(n == -1){
		printf("Erro: %s\n", strerror(errno));
		exit(1);//error
	}
	
	char new[128] = "NEWz ";//para que seja possivel identificar que o join é do tipo join_succi é enviado um caracter junto à palavra NEW
	char buffer[128] = " ";
	char my_porto[128];
	
	sprintf(my_porto, "%d", myport);
	strcat(new, no_x);
	strcat(new, " ");
	strcat(new, getip(buffer));
	strcat(new, " ");
	strcat(new, my_porto);
	
	no->arranque = 0;
	strcpy(no->id, no_x);
	strcpy(no->IP, buffer);
	strcpy(no->TCP, my_porto);
	strcpy(no->id_succi, suc);
	strcpy(no->succi_IP, suc_ip);
	strcpy(no->succi_TCP, suc_tcp);
	strcpy(no->id_predi, suc);
	strcpy(no->predi_IP, suc_ip);
	strcpy(no->predi_TCP, suc_tcp);
	n = write(fd,new,strlen(new));
	sprintf(desc, "%d", fd);
	return ;
}
	
		
int main(int argc, char *argv[]){
	
	int boot_port;
	int ring_port;
	int i, j, opt1, opt2, ctr = 0;	
	char buffer[128];
	char buffer2[128];
	char buffer3[128];
	char abc_ip[128];
	char abc_port[128];
	char a[32], b[32], c[32], d[32], e[32], f[32];
	char desc [128] = " -1";
	struct info_no no;
	
		if(argc < 3){
			printf("Too few arguments\n");
			exit(0);
		}
		if(strcmp(argv[argc-1], "-i") == 0){
			get_tejo_ip(buffer);
			opt1 = 1;
		}
		if(strcmp(argv[argc-1], "-p") == 0){
			boot_port = 58000;
			opt2 = 1;
		}
		if(strcmp(argv[argc-1], "-t") == 0){
			printf("Bad argument\n");
			exit(0);
		}
		for(i = 0; i < argc; i++){
			if(strcmp(argv[i], "-t") != 0){
				ctr++;
			}if(ctr == argc){
				printf("Bad argument\n");
				exit(0);
			}
		}
		ctr = 0;
		for(i = 0; i < argc; i++){
			if(strcmp(argv[i], "-i") != 0){
				ctr++;
			}if(ctr == argc){
				get_tejo_ip(buffer);
				opt1 = 1;
			}
		}
		ctr = 0;
		for(i = 0; i < argc; i++){
			if(strcmp(argv[i], "-p") != 0){
				ctr++;
			}if(ctr == argc){
				boot_port = 58000;
				opt2 = 1;
			}
		}
		
		for(i = 0; i < argc; i++){
			if(strcmp(argv[i], "-t") == 0){
				if(atoi(argv[i+1]) > 0 && atoi(argv[i+1]) < 99999999){
					ring_port = atoi(argv[i+1]);
				}else{
					printf("Bad port. Must be valid integer\n");
					exit(0);
				}
			}
			if(strcmp(argv[i], "-i") == 0 && opt1 != 1){
				strcpy(buffer3, argv[i+1]);
				for(j = 0; j < strlen(buffer3); j++){
					if(buffer3[j] > 'A' && buffer3[j] < 'z'){
						printf("Bad IP address\n");
						exit(0);
					}else{
						strcpy(buffer,argv[i+1]);
					}
				}
			}
			if(strcmp(argv[i], "-p") == 0 && opt2 != 1){
				if(atoi(argv[i+1]) > 0 && atoi(argv[i+1]) < 99999999){
					boot_port = atoi(argv[i+1]);
				}else{
					printf("Bad port. Must be valid integer\n");
					exit(0);
				}
			}
		} 
	
	// MAIN LOOP
	while(1){
		printf("Escreva <join x i> para se juntar a um anel automaticamente.\n");
		printf("Escreva <join x i succi succi.ip succi.tcp> para se juntar a um anel conhecendo succi.\n");
		printf("Escreva <exit> para sair.\n");
		fgets(buffer2, 128, stdin);
		if(strcmp(buffer2, "exit\n") == 0) break;
		if(strncmp(buffer2, "join", 4) == 0){
			if(sscanf(buffer2, "%s %s %s %s %s %s", a, b, c, d, e, f) == 6){ 
				reg_succi(b, c, d, e, f, &no, ring_port, desc);
			}else if(sscanf(buffer2, "%s %s %s", a, b, c) == 3){
				reg(buffer2, buffer, boot_port, ring_port, 0, &no, desc, abc_ip, abc_port);	
			}
		select_tcp(0, atoi(desc),-1, -1, ring_port,abc_ip, atoi(abc_port), &no, buffer, boot_port);
		}	
	}
	
	exit(0);
}
