#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]){

int i, bad_arguments=0, contagem=0; //Integers to check the arguments
int porto_maquina;
char surname[20], ip_maquina[20];


//Check the application's arguments

//saip and saport omitted, argc=7 (snp -n surname -s snpip -q snpport)
if(argc==7){
	for(i=1;i<7;i+=2){
		if(strcmp(argv[k],"-n")==0){//Surname
			strcpy(surname, argv[k+1]);
			contagem++;
		}else if(strcmp(argv[k],"-s")==0){//IP address
			strcpy(ip_maquina, argv[k+1]);
			contagem++;
		}else if(strcmp(argv[k],"-q")==0){//UDP Port
			porto_maquina=atoi(argv[k+1]);
			contagem++;
		}
	}
	char saip[30] = "tejo.tecnico.ulisboa.pt";//Valores por omissão
	int saport = 58000;
	if(contagem != 3){bad_arguments=1;}//Wrong arguments
}
//All arguments
else if(argc==11){
	for(i=1;i<11;i+=2){
		if(strcmp(argv[k],"-n")==0){//Surname
			strcpy(surname, argv[k+1], strlen(apelido));
			contagem++;
		}else if(strcmp(argv[k],"-s")==0){//IP address
			contagem++;
		}else if(strcmp(argv[k],"-q")==0){//UDP Port
			contagem++;
		}else if(strcmp(argv[k],"-i")==0){//IP address of surname's server
			contagem++;
		}else if(strcmp(argv[k],"-p")==0){//Port of surname's server
			contagem++;
		}
	}
	if(contagem != 5){bad_arguments=1;}
}

//Missing arguments , too many arguments or bad arguments
if((argc!=7 && argc!=11) || bad_arguments == 1){
	printf("Invocar aplicação da seguinte forma ./snp -n surname -s snpip -q snpport [-i saip] [-p saport]\n"); 
	exit(1);
}

//Loop List/Exit
 
}

