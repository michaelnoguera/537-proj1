#include "linkedlist.h"

#include <stdio.h>
#include <assert.h>

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
    if (list == NULL) {
        perror("Can't add an element to NULL.");
        exit(EXIT_FAILURE);
    }

    // construct node to add
    node* new = (node*)malloc(sizeof(node));
    if (new == NULL) {
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
int ll_get(const linkedlist* list, int index) {
    if (list == NULL) {
        perror("Can't get an element from NULL.");
        exit(EXIT_FAILURE);
    }
    
    if (list->size <= index) return -1;

    node* curr = list->head;
    for (int i = 0; i < index; i++) {
        curr = curr->next;
    }

    return curr->value;
}

// remove a node from the list
// return 1 if successful
// return 0 if no such node exists
/*int ll_remove(linkedlist* list, int value) {
    if (list == NULL) {
        perror("Can't remove an element from NULL.");
        return;
    }
    
    if (list->head == NULL) return 0; // list has no elements

    node* curr = list->head;
    node* prev = NULL;
    
    int i = 0;
    while (curr->value != value && i < list->size) {
        prev = curr;
        curr = curr->next;
        assert (curr != NULL); // list should be circularly linked
        i++;
    }

    if (prev = NULL) { // list has one node that is the target

    }

    free(list->head);
    list->head == NULL;
    list->tail == NULL;
    list->size == 0;

}*/

// free all memory from the specified linkedlist
// if ptr is null, no operation is performed
void ll_free(linkedlist* ptr) {
    if (ptr == NULL) return;
    
    // free all nodes
    node* tmp = NULL;
    while (ptr->head != ptr->tail) {
        tmp = ptr->head;
        ptr->head = ptr->head->next;
        free(tmp);
    }
    free(ptr->tail);

    // free list wrapper
    free(ptr);
}

// print out all values in list 
void ll_print(const linkedlist* list) {
    node* curr = list->head;

    for (int i = 0; i < list->size; i++) {
        printf("%d, ", curr->value);
        curr = curr->next;
    }
    printf("\n");
}