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
#include "teste.h"
#include <arpa/inet.h>
#define max(A,B) ((A)>=(B)?(A):(B))

int unreg(char * ip, int boot_port, int tcp_port, struct info_no * no, int succi_index, int predi_index){
	
	int rec, send, addrlen, fd;
	char cmd[128];
	char unreg[128] = "UNR ";
	char buffer[128];
	struct sockaddr_in addr;
	struct hostent *h;
	struct in_addr *a;
	
	addrlen = sizeof(addr);
	
	fd=socket(AF_INET,SOCK_DGRAM,0);//UDP socket
	if(fd==-1)exit(1);//error	
	if((h=gethostbyname(ip))==NULL){
		printf("Erro: %s\n", strerror(errno));
		exit(1);
	}
	a=(struct in_addr*)h->h_addr_list[0];
	memset((void*)&addr,(int)'\0',sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr=*a;
	addr.sin_port=htons(boot_port); //udp port
		
	strcat(unreg, no->anel);
	strcpy(cmd, unreg);
	
	if((succi_index == -1) && (predi_index == -1)){	
			send=sendto(fd,cmd,strlen(cmd),0, (struct sockaddr*)&addr, sizeof(addr));		
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
				write(1,"Resposta: ",10);//stdout
				write(1,buffer,rec);
				printf("\n");
			}
			return 0;
	}else{
			if(no->arranque == 1){ 
			char reg[128] = "REG "; 
			strcat(reg, no->anel);
			strcat(reg, " ");
			strcat(reg, no->id_succi);
			strcat(reg, " ");
			strcat(reg, no->succi_IP);
			strcat(reg, " ");
			strcat(reg, no->succi_TCP);
			bzero(cmd,128);
			strcpy(cmd, reg);
			
			send=sendto(fd,cmd,strlen(cmd),0, (struct sockaddr*)&addr, sizeof(addr));		
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
				write(1,"Resposta: ",10);//stdout
				write(1,buffer,rec);
				printf("\n");
			}
			return 1;
		}
	}
	return 3;
}

//Funcao que seleciona e interpreta os diferentes inputs provenientes tanto do teclado(stdin) como das comunicaçoes via TCP
void select_tcp(int flag, int desc, int flag_predi, int flag_succi, int tcp_port, char * ip2, int tcp_port1, struct info_no * no, char * udp_ip, int udp_port){
    int listen_desc, conn_desc = -1; // main listening descriptor and connected descriptor
    int maxfd; // max value descriptor and index in client array
    fd_set tempset;  // descriptor set to be monitored
    struct sockaddr_in serv_addr, client_addr;
    char buffer[128];
    char buffer1[128];
    char buffer_resposta[128];
    int num_bytes_succi,num_bytes_predi,num_bytes_conn, num_bytes1, numready;
    int predi_index = flag_predi;
    int succi_index = flag_succi;
    int flag_doing_join = 1;//verificar se é correcto
    int flag_CON = 0; 
    int flag_search = 0;
    int ret_unreg;
    listen_desc = socket(AF_INET, SOCK_STREAM, 0);

    if(listen_desc < 0){
        printf("Erro: %s\n", strerror(errno));
        exit(1);
	}


    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(tcp_port);

    if (bind(listen_desc, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        printf("Erro: %s\n", strerror(errno));
        exit(1);
	}

    listen(listen_desc, 5);
    maxfd = listen_desc; // Initialize the max descriptor with the first valid one we have
        if( desc != -1){ predi_index = desc; succi_index = desc;}
    FD_ZERO(&tempset); // initialize the descriptor set to be monitored to empty
    FD_SET(listen_desc, &tempset); // add the current listening descriptor to the monitored set
	FD_SET(fileno(stdin), &tempset);
	
	printf("Escreva <show> para imprimir o anel, o seu ID, bem como os ID do seu predi e succi.\n");
	printf("Escreva <search k> para realizar uma pesquisa pelo no responsavel por k.\n");
	printf("Escreva <leave> para abandonar o anel a que pertence\n");
	
	
    while(1){ // main server loop
		printf("waiting\n");
        
		FD_ZERO(&tempset); // initialize the descriptor set to be monitored to empty
		FD_SET(fileno(stdin), &tempset);
		maxfd=0;
		FD_SET(listen_desc, &tempset); // add the current listening descriptor to the monitored set
		maxfd=max(maxfd, listen_desc);
		
		if(predi_index > 0 && predi_index != succi_index){
			FD_SET(predi_index, &tempset);
			maxfd=max(maxfd, predi_index);
		}
		if(succi_index > 0){
			FD_SET(succi_index, &tempset);
			maxfd=max(maxfd, succi_index);
		}
		if(conn_desc > 0){
			FD_SET(conn_desc, &tempset);
			maxfd=max(maxfd, conn_desc);
		}
		//If indexs value is <0 the descriptor is not loaded to the monitored set
        numready = select(maxfd+1, &tempset, NULL, NULL, NULL); 

		if(numready<=0){ 
			printf("Erro: %s\n", strerror(errno));
			exit(1);
			}
		
		if(FD_ISSET(listen_desc, &tempset)){ // new client connection
            int size = sizeof(client_addr);
            conn_desc = accept(listen_desc, (struct sockaddr *)&client_addr, &size);
            
			if( (num_bytes_conn = read(conn_desc, buffer, 128)) != 0){
				
				buffer[num_bytes_conn] = '\0';
				printf("Received_listen:%s\n", buffer);
				
				char b[32];
				char c[32];
				sscanf(buffer, "%s %s", b, c);
					 
				if(strcmp(b, "ID") == 0 && strcmp(c, no->id) == 0){
					num_bytes1 = write(conn_desc,"Escolher outro no\n",18);
					
					break;
				}
				
					
				if( (strncmp(buffer, "ID", 2) == 0 && strcmp(c, no->id) != 0) ){
					flag_doing_join = 0;
					strcpy(buffer_resposta, tcp_searching(buffer, no, flag_search));
					
					if(strncmp(buffer_resposta, "QRY", 3) == 0){
						num_bytes1 = write(succi_index, buffer_resposta, strlen(buffer_resposta));
					}
					else if(strncmp(buffer_resposta, "RSP", 3) == 0){
						num_bytes1 = write(predi_index, buffer_resposta, strlen(buffer_resposta));
					}
					else if(strncmp(buffer_resposta, "SUCC", 4) == 0){
						num_bytes1 = write(conn_desc, buffer_resposta, strlen(buffer_resposta));

						conn_desc = -1; 
					} 
				}else if(strncmp(buffer, "NEW", 3) == 0){
					if(predi_index == -1 && succi_index == -1){
						flag_doing_join = 0;
					}
					if(buffer[3]!= ' ') flag_doing_join = 0;//sinalização de que o NEW que recebe é proveniente de um join succi.				
					if(flag_doing_join == 0){
						if(succi_index == predi_index){
							if(succi_index == -1 && predi_index == -1){						
								connecting_new_to_arranque(buffer, no);
								predi_index = conn_desc;
								succi_index = conn_desc;
								conn_desc = -1;
							}else{
								bzero(buffer_resposta, 128);
								strcpy(buffer_resposta, connections(buffer, no));
								num_bytes1 = write(predi_index, buffer_resposta, strlen(buffer_resposta));
								printf("Sent_predi:%s\n", buffer_resposta);
								predi_index = conn_desc;
								
							}
						}else if((predi_index == -1) && (succi_index > 0)){
							predi_index = conn_desc;
							conn_desc = -1;
							connecting_new(buffer, no);
						}else{
							bzero(buffer_resposta, 128);
							strcpy(buffer_resposta, connections(buffer, no));
							num_bytes1 = write(predi_index, buffer_resposta, strlen(buffer_resposta));
							printf("Sent_predi:%s\n", buffer_resposta);
							predi_index = conn_desc;
															
						}
					}else{
						connecting_new(buffer, no);
						predi_index = conn_desc;
						conn_desc = -1; 
						flag_doing_join = 0;
					}
				}
			}
			bzero(buffer,128);
		}
		
		bzero(buffer, 128);
		bzero(buffer_resposta, 128);
		
		
		
		if(FD_ISSET(fileno(stdin), &tempset)){
			if (fgets(buffer1,128, stdin)){
				printf("Input Command: %s\n",buffer1);
				
				if(strncmp(buffer1, "show", 4) == 0){
					printf("ID:%s\n", no->id);
					printf("IP:%s\n", no->IP);
					printf("TCP:%s\n", no->TCP);
					printf("ID_succi:%s\n", no->id_succi);
					printf("IP_succi:%s\n", no->succi_IP);
					printf("TCP_succi:%s\n", no->succi_TCP);
					printf("ID_predi:%s\n", no->id_predi);
					printf("IP_predi:%s\n", no->predi_IP);
					printf("TCP_predi:%s\n", no->predi_TCP);
					
				}else if(strncmp(buffer1, "search", 6) == 0 && strlen(buffer1) > 6){
					char temp [128];
					char no_procurado [128];
					char * prt;
					int responsabilidade = 0;
					strcpy(temp, buffer1);
					prt = strtok (temp, " ");
					prt = strtok (NULL, " ");
					
					strcpy(temp, prt);
					strcpy(no_procurado, temp);
					responsabilidade = search_check(atoi(no->id), atoi(no_procurado), atoi(no->id_predi));
					if(predi_index == -1 && succi_index == -1)printf("Search result: %s %s:%s\n", no->id, no->IP, no->TCP);
					if(responsabilidade == 1 && (predi_index != -1 && succi_index != -1)) printf("Search result: %s %s:%s\n", no->id, no->IP, no->TCP);
					else if(responsabilidade == 0 && (predi_index != -1 && succi_index != -1)){
						char QRY[128] = "QRY ";
						strcat(QRY, no->id);
						strcat(QRY, " ");
						strcat(QRY, temp);
						flag_search = 1;
						num_bytes1 = write(succi_index, QRY, strlen(QRY));	
						printf("Sent_succi: %s\n", QRY);		
					}					
				}else if(strncmp(buffer1, "leave", 5) == 0){
					if(no->arranque == 1){
						ret_unreg = unreg(udp_ip, udp_port, 00000, no, succi_index, predi_index);
						
						if(ret_unreg == 1){
							if(strcmp(no->id_predi, no->id_succi) == 0){
								num_bytes1 = write(succi_index, "BOOT", 4);
								close(listen_desc);
								return;
							}else{
								num_bytes1 = write(succi_index, "BOOT", 4);
								char conz[128] = "CON ";
								strcat(conz, no->id_succi);
								strcat(conz, " ");
								strcat(conz, no->succi_IP);
								strcat(conz, " ");
								strcat(conz, no->succi_TCP);
								num_bytes1 = write(predi_index, conz, strlen(conz));
								
								close(listen_desc);
								return;
							}
							
						}else if(ret_unreg == 0){
							
							close(listen_desc);
							return;
						}
					}else if(no->arranque == 0){
						
						if(predi_index != succi_index){
							
							char conz[128] = "CON ";
							strcat(conz, no->id_succi);
							strcat(conz, " ");
							strcat(conz, no->succi_IP);
							strcat(conz, " ");
							strcat(conz, no->succi_TCP);
							num_bytes1 = write(predi_index, conz, strlen(conz));
							close(listen_desc);
							return;	
						}else{
							char conz[128] = "CON ";
							strcat(conz, no->id_succi);
							strcat(conz, " ");
							strcat(conz, no->succi_IP);
							strcat(conz, " ");
							strcat(conz, no->succi_TCP);
							num_bytes1 = write(predi_index, conz, strlen(conz));
							close(listen_desc);
							return;
						}				
					}
				}else{
					printf("Unknown Command\n");
				}
			}
		}	
			

		bzero(buffer, 128);
		bzero(buffer_resposta, 128);


	
		if(FD_ISSET(succi_index, &tempset)){
			bzero(buffer, 128);
			if( (num_bytes_succi = read(succi_index, buffer, 128)) != 0){
				
				buffer[num_bytes_succi] = '\0';
				printf("Received_succi:%s\n", buffer);
				flag_doing_join = 0;
				
				if(strncmp(buffer, "CON", 3) == 0){
					int n1;
					flag_CON = 0;
					bzero(buffer_resposta, 128);
										
					char a [128];
					char b [128];
					sscanf(buffer, "%s %s", a, b);
					if((strcmp(no->id_succi, no->id_predi) == 0) && (strcmp(b, no->id) == 0)){
						strcpy(no->id_predi, no->id);
						strcpy(no->predi_IP, no->IP);
						strcpy(no->predi_TCP, no->TCP);
						strcpy(no->id_succi, no->id);
						strcpy(no->succi_IP, no->IP);
						strcpy(no->succi_TCP, no->TCP);	
						predi_index = -1;
						succi_index = -1;
						
					}else if(strcmp(b, no->id_predi) == 0){
						
						succi_index = predi_index;
						strcpy(buffer_resposta, connections(buffer, no));
						n1 = write(succi_index, buffer_resposta, strlen(buffer_resposta));
						printf("Sent_succi:%s\n", buffer_resposta);
						
					}else{
	
						
						int con;
						struct ip_addr1;
						struct sockaddr_in addr1, ip_addr1;
						char ip1[128];
					
						con = socket(AF_INET, SOCK_STREAM, 0);//TCP socket	
						if(con == -1){
							printf("Erro: %s\n", strerror(errno));
							exit(1);//error
						}
						
						strcpy(buffer_resposta, connections(buffer, no));

						strcpy(ip1, no->succi_IP);
						inet_aton(ip1, &ip_addr1.sin_addr); 
						
						memset((void*)&addr1, (int)'\0', sizeof(addr1));
						addr1.sin_family = AF_INET;
						addr1.sin_addr = ip_addr1.sin_addr;
						addr1.sin_port = htons(atoi(no->succi_TCP));
						
						n1 = connect(con,(struct sockaddr*)&addr1, sizeof(addr1));
						
						if(con == -1){
							printf("Erro: %s\n", strerror(errno));
							exit(1);//error
						}

						
						
						succi_index = con;
						n1 = write(succi_index, buffer_resposta, strlen(buffer_resposta));
						printf("Sent_succi:%s\n", buffer_resposta);
						if(con > maxfd)
						maxfd = succi_index;
					}
				
				}else if(strncmp(buffer, "RSP", 3) == 0){
					
					bzero(buffer_resposta, 128);
					strcpy(buffer_resposta, tcp_searching(buffer, no, flag_search));
					if(strncmp(buffer_resposta, "RSP", 3) == 0){
						num_bytes1 = write(predi_index, buffer_resposta, strlen(buffer_resposta));
					}
					
					else if(strncmp(buffer_resposta, "SUCC", 4) == 0){
						num_bytes1 = write(conn_desc, buffer_resposta, strlen(buffer_resposta));
						conn_desc = -1;
					}
					else if(flag_search == 1){
						flag_search = 0;
						printf("Search result:%s\n", buffer_resposta);
					}
				}else if(strncmp(buffer, "QRY", 3) == 0){				
					strcpy(buffer_resposta, tcp_searching(buffer, no, flag_search));
					if(strncmp(buffer_resposta, "QRY", 3) == 0){
						num_bytes1 = write(succi_index, buffer_resposta, strlen(buffer_resposta));
					}
					else if(strncmp(buffer_resposta, "RSP", 3) == 0){
						num_bytes1 = write(predi_index, buffer_resposta, strlen(buffer_resposta));
						
					}
				}else if(strncmp(buffer, "NEW", 3) == 0){
					predi_index = succi_index;
					strcpy(no->id_predi, no->id_succi);
					strcpy(no->predi_IP, no->succi_IP);
					strcpy(no->predi_TCP, no->succi_TCP);				
				}else if((strncmp(buffer, "BOOT", 4) == 0) && (predi_index == succi_index)){
					predi_index = -1;
					succi_index = -1;
					no->arranque = 1;
					strcpy(no->id_succi,no->id);
					strcpy(no->id_predi,no->id);
					strcpy(no->predi_IP,no->IP);
					strcpy(no->predi_TCP,no->TCP);
					strcpy(no->succi_IP,no->IP);
					strcpy(no->succi_TCP,no->TCP);
					}
				
			}
		}
			
		bzero(buffer, 128);
		bzero(buffer_resposta, 128);
	
		if(FD_ISSET(predi_index, &tempset) && (flag_CON != 0)){
			if(strcmp(no->id_predi, no->id_succi) != 0){
				
				if( (num_bytes_predi = read(predi_index, buffer, 128)) != 0){
					
					buffer[num_bytes_predi] = '\0';
					printf("Received_predi:%s\n", buffer);			
					flag_doing_join = 0;	
							
					if(strncmp(buffer, "QRY", 3) == 0){
						char temp9 [128];
						char no_procurado9 [128];
						char prt9 [128];
						int responsabilidade9 = 0;
						
						sscanf(buffer, "%s %s %s", temp9, prt9, no_procurado9);
						
						responsabilidade9 = search_check(atoi(no->id), atoi(no_procurado9), atoi(no->id_predi));
						if(responsabilidade9 == 1){
							char rsp9[128] = "RSP ";			
							strcat(rsp9, prt9);		
							strcat(rsp9, " ");
							strcat(rsp9, no_procurado9);
							strcat(rsp9, " ");
							strcat(rsp9, no->id);		
							strcat(rsp9, " ");
							strcat(rsp9, no->IP);
							strcat(rsp9, " ");
							strcat(rsp9, no->TCP);	
							num_bytes1 = write(predi_index, rsp9, strlen(rsp9));
						}
						else if(responsabilidade9 == 0){
								strcpy(buffer_resposta, tcp_searching(buffer, no, flag_search));
							
							if(strncmp(buffer_resposta, "QRY", 3) == 0){
								
								num_bytes1 = write(succi_index, buffer_resposta, strlen(buffer_resposta));
							}
							
							else if(strncmp(buffer_resposta, "RSP", 3) == 0){
								
								num_bytes1 = write(predi_index, buffer_resposta, strlen(buffer_resposta));
								
							}
						}	
					}else if((strncmp(buffer, "BOOT", 4) == 0) && (predi_index == succi_index)){
						predi_index = -1;
						succi_index = -1;
						no->arranque = 1;
						strcpy(no->id_succi,no->id);
						strcpy(no->id_predi,no->id);
						strcpy(no->predi_IP,no->IP);
						strcpy(no->predi_TCP,no->TCP);
						strcpy(no->succi_IP,no->IP);
						strcpy(no->succi_TCP,no->TCP);
						
					}else if((strncmp(buffer, "BOOT", 4) == 0) && (predi_index != succi_index)){
						no->arranque = 1;
						predi_index = -1;	
						strcpy(no->id_predi,no->id);
						strcpy(no->predi_IP,no->IP);
						strcpy(no->predi_TCP,no->TCP);
										
					}
				}
			}
			if( num_bytes_predi  == 0 && (predi_index != succi_index)){
				close(predi_index); 
				predi_index = -1;
			}			
		}
	
		bzero(buffer, 128);
		bzero(buffer_resposta, 128);		
		flag_CON = 1;

    } // End main listening loop

    close(listen_desc);
}

