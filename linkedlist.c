#include "linkedlist.h"

#include <stdio.h>

// initializes empty list
// remember to call ll_free when done!
linkedlist* ll_initialize() {
    linkedlist* list = (linkedlist*)malloc(sizeof(linkedlist));
    if (list == NULL) {
        perror("Error allocating memory for new list.\n");
        exit(EXIT_FAILURE);
    }

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;

    return list;
}

// adds new node with value at end
void ll_push(linkedlist* list, int value) {
    // construct node to add
    node* new = (node*)malloc(sizeof(node));
    if (new == NULL// adds new node with value at end
) {
        perror("Error allocating memory for new node.\n");
        exit(EXIT_FAILURE);
    }
    new->value = value;

    // attach node to tail of list
    if (list->size == 0) {
        list->head = new;
        list->tail = new;
    } else {
        list->tail->next = new;
        list->tail = new;
    }
    list->size++;

    new->next = list->head;  // complete circular linking
}

// gets value of node at index
// returns -1 if no such node exists
int ll_get(linkedlist* list, int index) {
    if (list->size <= index) return -1;

    node* curr = list->head;
    for (int i = 0; i < index; i++) {
        curr = curr->next;
    }

    return curr->value;
}

// free all memory from the specified linkedlist
// if ptr is null, no operation is performed
void ll_free(linkedlist* ptr) {
    if (ptr == NULL) return;
    
    // free all nodes
    node* curr = ptr->head;
    node* next = NULL;
    while (ptr->size > 0) {
        next = curr->next;
        free(curr);
        curr = next;
    }

    free(ptr);
}

// print out all values in list 
void ll_print(linkedlist* list) {
    node* curr = list->head;

    for (int i = 0; i < list->size; i++) {
        printf("%d, ", curr->value);
        curr = curr->next;
    }
    printf("\n");
}