#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

typedef struct element_{
    char ip[20], surname[20], name[20];
    int port;
    struct element_ * next;
}element;

// Function to add new elements in an alphabetically sorted way
element * addElement(char name[20], char surname[20],
                        char ip[20], element * ptr_to_first) {
        element * current, *previous, * new_element = (element*)malloc(sizeof(element));
        //Copy the strings given into the new element to add.
        strcpy(new_element->name, name);
        strcpy(new_element->surname, surname);
        strcpy(new_element->ip, ip);
        // Initialize pointers. Previous is used in order not to loose the element previous to the current in case of inserting before current.
        current = ptr_to_first;
        previous = ptr_to_first;
        if (ptr_to_first==NULL) {           // Check if the list is empty.
            ptr_to_first = new_element;
            new_element->next = NULL;
        }else {                             // In case it is not empty.
            while (current!=NULL) {
                if ((current->next ==NULL) && (previous==(current))) {      // Check if current is the last AND the only element on the list.
                     if (strcmp(name,current->name)>0){                     // Determines if the new element should be placed before or
                        current->next = new_element;                        // after the current element according to its name.
                        new_element->next = NULL;
                     }else {
                        new_element->next = current;
                        ptr_to_first = new_element;                        // If it is inserted before, the new head pointer of the list
                     }                                                      // is now the pointer to the new element.
                      return ptr_to_first;
                } else{                                                     // List has multiple elements
                         if (strcmp(name,current->name)>0) {
                            previous = current;
                            if (current->next!=NULL) {                      // If it is NOT the last on the list then continues.
                                previous = current;
                                current = current->next;
                            } else {
                                current->next = new_element;
                                new_element->next = NULL;
                                return ptr_to_first;
                            }
                         }else {                                      // If the new name is smaller than the current name, it adds it before.
                                new_element->next = current;
                                if (ptr_to_first == current) {
                                    ptr_to_first = new_element;
                                }else{
                                previous->next = new_element;
                                }return ptr_to_first;

                         }
            }
        }
        }
return ptr_to_first;
}

// Function to print the correspondence between full name and IP address in a name server.
void printList(element * ptr_to_first) {
     element * current = ptr_to_first;
     int i =0;
     if (current==NULL) {
        printf("Empty list!\n");
        }else {
            while (current!=NULL) {
                printf("%s %s %s\n", current->name, current->surname, current->ip);
                current=current->next;
            }
        }
        return;
}

// Cleaning memory at exit
void freeList(element * ptr_to_first)
{
   element * tmp;

   while (ptr_to_first != NULL)
    {
       tmp = ptr_to_first;
       ptr_to_first = ptr_to_first->next;
       free(tmp);
    }
}

// Function to delete one entry [name surname]
element * deleteElement(element * ptr_to_first, char name[20], char surname[20]) {
    element * previous, *current = ptr_to_first;
    previous = current;
    while (current!=NULL) {
        if (strcmp(current->name,name)==0 && strcmp(current->surname,surname)==0) {
            if (current->next==NULL && previous==current) {
                ptr_to_first = NULL;
                free(current);
                return ptr_to_first;
            }else {
                if (previous==current){
                    ptr_to_first = current->next;
                    free(current);
                    return ptr_to_first;
                }else {
                    previous->next = current->next;
                    free(current);
                    return ptr_to_first;
                }
            }
        }
        previous = current;
        current = current->next;
    }
    return ptr_to_first;
}

int main(int argc, char * argv[]){

    int i,j,bad_arguments=0, contagem=0; //Integers to check the arguments
    int porto_maquina = 0,porto_servidor = 0;
    char surname[20], ip_maquina[20], ip_servidor[20];

    struct in_addr ipaddress, ipaddress2;


    //Check the application's arguments

    //saip and saport omitted, argc=7 (snp -n surname -s snpip -q snpport)
    if(argc==7){
        for(i=1;i<7;i+=2){
            if(strcmp(argv[i],"-n")==0){//Surname
                if(strlen(argv[i+1])>20){//Check the lenght of the surname
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
                }
            }else if(strcmp(argv[i],"-s")==0){//IP address
                if(inet_pton(AF_INET,argv[i+1],&(ipaddress.s_addr))==1){//Check if the ip is valid
                    strcpy(ip_maquina, argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid IP Address\n");
                }
            }else if(strcmp(argv[i],"-q")==0){//UDP Port, valor dos portos está entre 1024 e 65535 (valor máximo é 65535 2^16-1 16 bits)  1024–49151 portos registados acima de 49151 are called dynamic and/or private ports
                if((49151 < atoi(argv[i+1])) && (atoi(argv[i+1])<= 65535)){//Check if the port is valid
                    porto_maquina=atoi(argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid Port: Value of Port must be between 49151 and 65535\n");
                    bad_arguments=1;
                }
            }
        }
        // char saip[30] = "tejo.tecnico.ulisboa.pt";//Omitted values
        //int port2 = 58000;
        if(contagem != 3){bad_arguments=1;}//Wrong arguments
    }
    //All arguments
    else if(argc==11){
        for(i=1;i<11;i+=2){
            if(strcmp(argv[i],"-n")==0){//Surname
                if(strlen(argv[i+1])>20){//Check the lenght of the surname
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
                }
            }else if(strcmp(argv[i],"-s")==0){//IP address
                if(inet_pton(AF_INET,argv[i+1],&(ipaddress.s_addr))==1){//Check if the ip is valid
                    strcpy(ip_maquina, argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid IP Address\n");
                }
            }else if(strcmp(argv[i],"-q")==0){//UDP Port
                if((49151 < atoi(argv[i+1])) && (atoi(argv[i+1])<= 65535)){//Check if the port is valid
                    porto_maquina=atoi(argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid Port: Value of Port must be between 49151 and 65535\n");
                    bad_arguments=1;
                }
            }else if(strcmp(argv[i],"-i")==0){//IP address of surname's server
                if(inet_pton(AF_INET,argv[i+1],&(ipaddress2.s_addr))==1){//Check if the ip is valid
                    strcpy(ip_servidor, argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid IP Address\n");
                }
            }else if(strcmp(argv[i],"-p")==0){//Port of surname's server
                if((49151 < atoi(argv[i+1])) && (atoi(argv[i+1])<= 65535)){//Check if the port is valid
                    porto_servidor=atoi(argv[i+1]);
                    contagem++;
                }else{
                    printf("Invalid Port: Value of Port must be between 49151 and 65535\n");
                    bad_arguments=1;
                }
            }
        }
        if(contagem != 5){bad_arguments=1;}//Wrong arguments
    }


    //Missing arguments , too many arguments or bad arguments
    if((argc!=7 && argc!=11) || bad_arguments == 1){
        printf("Invocar aplicação da seguinte forma ./snp -n surname -s snpip -q snpport [-i saip] [-p saport]\n");
        exit(1);
    }


    //Registo do servidor de nomes,envio da sua localizaçã para o servidor de apelidos
    //Loop List/Exit
    printf("Apelido: %s\nIP address: %s\nPort: %d\n",surname,ip_maquina,porto_maquina);


    // Linked list with elements with Full Name <--> IP address
    element *ptr_to_first;
    char n_name[20], n_surname[20], n_ip[20];
    ptr_to_first = NULL;

    // Interface with the user
    int interface_option;
    while(1){

        printf("\nWelcome to the Name Server Interface. Choose an action: \n 1: List\n 2: Exit \n 3: Add to list \n 4: Remove from list \n Action nr: ");
        scanf("%d",&interface_option);

        switch (interface_option) {
            case 1:
                printf("\n Complete table of association [Full name] <--> [IP]<\n");
                printList(ptr_to_first);
                break;
            case 2:
                printf("\n Terminating server... \n");
                freeList(ptr_to_first);
                //free(ptr_to_first);
                exit(0);
                break;
            // NOT PART OF THE INTERFACE. Just for testing the linked list.
            case 3:
                printf("\n Write in format [Name Surname IP] to be added: ");
                scanf("%s", n_name);
                scanf("%s", n_surname);
                scanf("%s", n_ip);
                ptr_to_first = addElement(n_name,n_surname, n_ip, ptr_to_first);
                break;
            case 4:
                printf("\n Write in format to delete from list [Name Surname]: ");
                scanf("%s", n_name);
                scanf("%s", n_surname);
                ptr_to_first = deleteElement(ptr_to_first, n_name, n_surname);
                break;
        }
    }
       exit(0);
}
