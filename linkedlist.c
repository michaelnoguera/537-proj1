/**
 * CS 537 Programming Assignment 1 (Fall 2020)
 * @author Michael Noguera, contributions by Julien de Castelnau
 * @date 9/23/2020
 * @file linkedlist.c
 */

#include "linkedlist.h"

#include <stdio.h>
#include <assert.h>

/**
 * Initializes a new empty linkedlist.
 * Remember to call ll_free when done!
 * 
 * @return pointer to new heap-allocated linkedlist
 */
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

/**
 * Adds a new node to the end of a linkedlist.
 * 
 * @param list The list to append to.
 * @param value The value of the new node.
 */
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

/**
 * Gets the value of the node at an index.
 * 
 * @param list The list to search.
 * @param index The index of the node to be retrieved. Lists are zero-indexed.
 * @return The value of the node, or NULL if no such node exists.
 */
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

/**
 * Frees all memory from the specified linkedlist.
 * If  `ptr` is NULL, no operation is performed.
 * 
 * @param ptr Pointer to linkedlist to be freed.
 */
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

/**
 * Prints all values from the specified linkedlist.
 * @author Julien de Castelnau
 * 
 * @param list The list to print.
 */
void ll_print(const linkedlist* list) {
    node* curr = list->head;

    for (int i = 0; i < list->size; i++) {
        printf("%d, ", curr->value);
        curr = curr->next;
    }
    printf("\n");
}