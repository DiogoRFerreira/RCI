#include <stdio.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include "struct_no.h"
#include <math.h>
#define h 6

//funcao que calcula a distancia entre dois nos
int dist(int k, int l){
	
	int d;
	if(l >= k){
		d = l-k;
	}else{	
		d = (int)pow(2,h)+(l-k);
	}
	return d;
}

//funcao que devolve o IP da maquina
char * getip1(char * cmd){
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
	strcpy(cmd, host);
    freeifaddrs(ifaddr);
	
		
    return cmd;
}


char * tcp_searching(char *searching, struct info_no *no, int flag_search){
	//ir à estrutura ver se é responsavel caso nao seja chamar client e enviar QRY
	//se receber BRSP verificar se é no de arranque caso nao seja enviar RSP
	
	if(strncmp(searching, "QRY", 3) == 0){

		char * pch;
		char no_arranque[128];
		char no_procurado[128];
		char buffer[128];
		int l, k, pred, responsabilidade = 0;
		int i=0;
		strcpy(buffer, searching);
		searching[strlen(searching)-1]= '\0';
		pch = strtok (searching," ");
		
		while (pch != NULL){
			i++;
			if(i == 2) strcpy(no_arranque, pch);
			if(i == 3) strcpy(no_procurado, pch);	
			pch = strtok (NULL, " ");
		}			
	
		l = atoi(no->id); 
		k = atoi(no_procurado); 
		pred = atoi(no->id_predi);
		
		if(dist(k, l) < dist(k, pred)){
			responsabilidade = 1;
		}
		
		if (responsabilidade == 1){
			
			char rsp[128] = "RSP ";			
			strcat(rsp, no_arranque);		
			strcat(rsp, " ");
			strcat(rsp, no_procurado);
			strcat(rsp, " ");
			strcat(rsp, no->id);		
			strcat(rsp, " ");
			strcat(rsp, no->IP);
			strcat(rsp, " ");
			strcat(rsp, no->TCP);	
			strcpy(searching, rsp);	
				
			return searching;
		}
		else{
			strcpy(searching, buffer);
			return searching;
		}

	}

	else if(strncmp(searching, "RSP", 3) == 0){
		char temp [128];
		char * prt;
		
		strcpy(temp, searching);
		prt = strtok (temp, " ");
		prt = strtok (NULL, " ");
		
		strcpy(temp, prt);
		
		if(strcmp(no->id, temp) == 0  && flag_search !=1){
			char * pch;
			char no[128];
			char ip[128];
			char tcp[128];
			char buff[128];
			int i=0;
			strcpy(buff, searching);
			pch = strtok (searching," ");
			while (pch != NULL){
				i++;
				if(i == 4) strcpy(no, pch); 
				
				if(i == 5) strcpy(ip, pch);
				
				if(i == 6) strcpy(tcp, pch);
					
				pch = strtok (NULL, " ");
			}	
			
			char succ[128] = "SUCC ";
			strcat(succ, no);
			strcat(succ, " ");					
			strcat(succ, ip);		
			strcat(succ, " ");				
			strcat(succ, tcp);
			bzero(searching, 128);
			strcpy(searching, succ);
					
			return searching;//responder para cliente que perguntou
		}else if(strcmp(no->id, temp) == 0 && flag_search == 1){
			char * pch2;
			char no2[128];
			char ip2[128];
			char tcp2[128];
			char buff2[128];
			int i=0;
			
			pch2 = strtok (searching," ");
			while (pch2 != NULL){
				i++;
				if(i == 4) strcpy(no2, pch2);
				
				if(i == 5) strcpy(ip2, pch2);
				
				if(i == 6) strcpy(tcp2, pch2);
					
				pch2 = strtok (NULL, " ");
			}
			
			bzero(buff2, 128);
			strcat(buff2, no2);
			strcat(buff2, " ");					
			strcat(buff2, ip2);		
			strcat(buff2, ":");				
			strcat(buff2, tcp2);		
			bzero(searching, 128);
			strcpy(searching, buff2);				
			return searching;
		}else{
			return searching;//reenviar para predi
		}
	}

	else if(strncmp(searching, "ID", 2) == 0){
		if(strcmp(no->id, no->id_predi) == 0 && strcmp(no->id, no->id_succi) == 0 ){
			
			char succ[128] = "SUCC ";
			strcat(succ, no->id);
			strcat(succ, " ");
			strcat(succ, no->IP);
			strcat(succ, " ");
			strcat(succ, no->TCP);
			bzero(searching, 128);
			strcpy(searching, succ);
			
			return searching; 
			//reponder directamente atraves do server
		}
		else{
			//verificar responsabilidade
			int l, k, pred, responsabilidade = 0;
			char * pch;
			char no_procurado[128];
			int i=0;
			pch = strtok (searching," ");
			while (pch != NULL){
				i++;
				if(i == 2) strcpy(no_procurado, pch);	
				pch = strtok (NULL, " ");
			}
						
			l = atoi(no->id); 
			k = atoi(no_procurado); 
			pred = atoi(no->id_predi);


			if(dist(k, l) < dist(k, pred)){
				responsabilidade = 1;
			}
		
			if(responsabilidade == 1){
				char succ[128] = "SUCC ";
				strcat(succ, no->id);
				strcat(succ, " ");
				strcat(succ, no->IP);
				strcat(succ, " ");
				strcat(succ, no->TCP);
				bzero(searching, 128);
				strcpy(searching, succ);
			
				return searching;
			}
			else{
				char qry[128] = "QRY ";
				strcat(qry, no->id);
				strcat(qry, " ");
				strcat(qry, no_procurado);
				bzero(searching, 128);
				strcpy(searching, qry);
				
				return searching;
				//responder para o succi
			}
		}
	}
	
	return NULL;	
}


char *connections(char * connection, struct info_no * no){
	if(strncmp(connection, "CON", 3) == 0){
		char * pch;
		char no1[128];
		char ip[128];
		char tcp[128];
		int i=0;
		pch = strtok (connection," ");
		while (pch != NULL){
			i++;
			if(i == 2) strcpy(no1, pch);
			
			if(i == 3) strcpy(ip, pch);
			
			if(i == 4) strcpy(tcp, pch);
				
			pch = strtok (NULL, " ");
		}
				
		strcpy(no->id_succi, no1);
		strcpy(no->succi_IP, ip);
		strcpy(no->succi_TCP, tcp);
		
			
		bzero(connection, 128);
		strcat(connection, "NEW");		
		strcat(connection, " ");		
		strcat(connection, no->id);		
		strcat(connection, " ");
		strcat(connection, no->IP);
		strcat(connection, " ");	
		strcat(connection, no->TCP);	
		
		return connection;
		
	}else if(strncmp(connection, "NEW", 3) == 0){
		char * pch;
		char no1[128];
		char ip[128];
		char tcp[128];
		char buffer[128]="CON ";
		int i=0;
		pch = strtok (connection," ");
		while (pch != NULL){
			i++;
			if(i == 2) strcpy(no1, pch);
			
			if(i == 3) strcpy(ip, pch);
			
			if(i == 4) strcpy(tcp, pch);
				
			pch = strtok (NULL, " ");
		}
		
		strcpy(no->id_predi, no1);
		strcpy(no->predi_IP, ip);
		strcpy(no->predi_TCP, tcp);		
		
		bzero(connection, 128);
		strcat(buffer, no1);
		strcat(buffer, " ");
		strcat(buffer, ip);
		strcat(buffer, " ");
		strcat(buffer, tcp);
		/*buffer[0] = 'C';
		buffer[1] = 'O';
		buffer[2] = 'N';
		bzero(connection, 128);*/
		
		strcpy(connection, buffer);
		
		return connection;
	}
	return NULL;
}


void connecting_new(char * connection, struct info_no * no){	
	char * pch;
	char no1[128];
	char ip[128];
	char tcp[128];
	char buffer[128];
	int i=0;
	strcpy(buffer, connection);
	pch = strtok (connection," ");
	while (pch != NULL){
		i++;
		if(i == 2) strcpy(no1, pch);
	
		if(i == 3) strcpy(ip, pch);
		
		if(i == 4) strcpy(tcp, pch);
			
		pch = strtok (NULL, " ");
	}
	
	strcpy(no->id_predi, no1);
	strcpy(no->predi_IP, ip);
	strcpy(no->predi_TCP, tcp);	
		
}


void connecting_new_to_arranque(char * connection, struct info_no * no){	
	char * pch;
	char no1[128];
	char ip[128];
	char tcp[128];
	char buffer[128];
	int i=0;
	strcpy(buffer, connection);
	pch = strtok (connection," ");
	while (pch != NULL){
		i++;
		if(i == 2) strcpy(no1, pch);
		
		if(i == 3) strcpy(ip, pch);
		
		if(i == 4) strcpy(tcp, pch);
			
		pch = strtok (NULL, " ");
	}
	
	strcpy(no->id_predi, no1);
	strcpy(no->predi_IP, ip);
	strcpy(no->predi_TCP, tcp);
	strcpy(no->id_succi, no1);
	strcpy(no->succi_IP, ip);
	strcpy(no->succi_TCP, tcp);		
		
}


int search_check(int l, int k, int predi){
	
	int responsabilidade = 0;

	if(dist(k, l) < dist(k, predi)){
		responsabilidade = 1;
	}
	
	return responsabilidade;
}
