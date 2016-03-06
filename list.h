#ifndef list_h
#define list_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

typedef struct element_{
    char ip[20], surname[20], name[20];
    int port;
    struct element_ * next;
}element;

element infotoelement2(char surname[], char ip[], int port);

element infotoelement(char name[], char surname[], char ip[], int port);

element *CheckInList(element * ptr_to_first, char name[20], char surname[20]);

element * addElement(element * ptr_to_first, element argElement, int * num_elements_ptr);

void printList(element * ptr_to_first);

void freeList(element * ptr_to_first);

element * deleteElement(element * ptr_to_first, char name[20], char surname[20], int * num_elements_ptr);


#endif /* list_h */
