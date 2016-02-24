#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to add new elements in an alphabetically sorted way
element * add_element(char name[20], char surname[20],char ip[20], element * ptr_to_first) {
    
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
                    ptr_to_first = new_element;                         // If it is inserted before, the new head pointer of the list
                }                                                      // is now the pointer to the new element.
                return ptr_to_first;
            }else{                                                     // List has multiple elements
                if (strcmp(name,current->name)>0) {
                    previous = current;
                    if (current->next!=NULL) {                              // If it is NOT the last on the list then continues.
                        previous = current;
                        current = current->next;
                    } else {
                        current->next = new_element;
                        new_element->next = NULL;
                        return ptr_to_first;
                    }
                }else{                                                    // If the new name is smaller than the current name, it adds it before.
                    new_element->next = current;
                    previous->next = new_element;
                    return ptr_to_first;
                    if (&previous==&current) {
                        ptr_to_first = new_element;
                    }
                    return ptr_to_first;
                    
                }
            }
        }
    }
    return ptr_to_first;
}



// Function to print the correspondence between full name and IP address in a name server.
void print_list(element * ptr_to_first) {
    element * current = ptr_to_first;
    if (current==NULL) {
        printf("Empty list!\n");
        return;
    }else {
        while (current!=NULL) {
            printf("%s %s %s\n", current->name, current->surname, current->ip);
            current=current->next;
        }
    }
    return;
}