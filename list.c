
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

element infotoelement2(char surname[], char ip[], int port) {
    element new_element;
    strcpy(new_element.surname, surname);
    strcpy(new_element.ip, ip);
    new_element.port = port;
    return new_element;
}

element infotoelement(char name[], char surname[], char ip[], int port) {
    element new_element;
    strcpy(new_element.name, name);
    strcpy(new_element.surname, surname);
    strcpy(new_element.ip, ip);
    new_element.port = port;
    return new_element;
}



element * CheckInList(element * ptr_to_first, char name[20], char surname[20]){

    element * current = ptr_to_first;

    if (current==NULL) {
        printf("Empty list => Not on the list\n");
    }else {
        while (current!=NULL) {
            if((strcmp(current->name,name) == 0) && (strcmp(current->surname,surname)==0)) {
                return current;
            }
            current=current->next;
        }

    }
    return NULL;
}

// Function to add new elements in an alphabetically sorted way
element * addElement(element * ptr_to_first, element argElement, int * num_elements_ptr) {


        element * current, *previous, * new_element = (element*)malloc(sizeof(element));

        //Copy the strings given into the new element to add.
        strcpy(new_element->name, argElement.name);
        strcpy(new_element->surname, argElement.surname);
        strcpy(new_element->ip, argElement.ip);
        new_element->port = argElement.port;
        printf("New register: %s %s %s %d\n", new_element->name,new_element->surname, new_element->ip, new_element->port);
        // Initialize pointers. Previous is used in order not to loose the element previous to the current in case of inserting before current.
        current = ptr_to_first;
        previous = ptr_to_first;
        if (ptr_to_first==NULL) {           // Check if the list is empty.
            ptr_to_first = new_element;
            new_element->next = NULL;
            (*num_elements_ptr)++;
            return ptr_to_first;
        }else{                             // In case it is not empty.
            while (current!=NULL) {
                if(strcmp(new_element->name,current->name)==0 && (strcmp(new_element->surname,current->surname)==0)) {
                    return ptr_to_first;
                } else{

                    if ((current->next ==NULL) && (previous==(current))) {      // Check if current is the last AND the only element on the list.
                         if (strcmp(new_element->name,current->name)>0){        // Determines if the new element should be placed before or
                            current->next = new_element;                        // after the current element according to its name.
                            new_element->next = NULL;
                         }else {
                            new_element->next = current;
                            ptr_to_first = new_element;                        // If it is inserted before, the new head pointer of the list
                         }                                                      // is now the pointer to the new element.
    (*num_elements_ptr)++;
return ptr_to_first;
                    } else{                                                     // List has multiple elements
                         if (strcmp(new_element->name,current->name)>0) {
                            previous = current;
                            if (current->next!=NULL) {                      // If it is NOT the last on the list then continues.
                                previous = current;
                                current = current->next;
                            } else {
                                current->next = new_element;
                                new_element->next = NULL;

                            }
                         }else {                                      // If the new name is smaller than the current name, it adds it before.
                                new_element->next = current;
                                if (ptr_to_first == current) {
                                    ptr_to_first = new_element;
                                }else{
                                previous->next = new_element;
                                }
                                    (*num_elements_ptr)++;
return ptr_to_first;

                        }
                    }
                }
            }
        }

return ptr_to_first;
}

// Function to delete one entry [name surname]
element * deleteElement(element * ptr_to_first, char name[20], char surname[20], int * num_elements_ptr){
    element * previous, *current = ptr_to_first;
    previous = current;
    while (current!=NULL) {
        if (strcmp(current->name,name)==0 && strcmp(current->surname,surname)==0) {
            if (current->next==NULL && previous==current) {
                ptr_to_first = NULL;
            }else {
                if (previous==current){
                    ptr_to_first = current->next;
                }else {
                    previous->next = current->next;
                }
            }
            (*num_elements_ptr)--;
            free(current);
            return ptr_to_first;
        }else{
        previous = current;
        current = current->next;
        }
    }
    return ptr_to_first;
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

// Function to print the correspondence between full name and IP address in a name server.
void printList(element * ptr_to_first) {

    element * current = ptr_to_first;

    if (current==NULL) {
        printf("Empty list!\n");
    }else {
        while (current!=NULL) {
            printf("%s %s %s %d\n", current->name, current->surname, current->ip, current->port);
            current=current->next;
        }
    }
    return;
}


