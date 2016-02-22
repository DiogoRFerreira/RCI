#ifndef list_h
#define list_h

#include <stdio.h>

typedef struct element_{
    char ip[20], surname[20], name[20];
    int port;
    struct element_ * next;
}element;

element * add_element(char name[20], char surname[20],char ip[20], element * ptr_to_first);

void print_list(element * ptr_to_first);

#endif /* list_h */
