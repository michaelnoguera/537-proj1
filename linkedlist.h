#include <stdlib.h>

#ifndef _LINKEDLIST_
#define _LINKEDLIST_

/** Node type for linkedlist_t, that holds a single integer value. */
typedef struct ll_node_t {
    int value; /**< the value stored in the node */ 
    struct ll_node_t* next; /**< pointer to the next node in the list */
} node;

/**
 * Implementation of heap-allocated circular singly-linked list with head 
 * and tail pointers. Order is not guaranteed when multiple nodes have the
 * same value, especially after remove operations.
 */
typedef struct linkedlist_t {
    int size; /**< number of items currently in the list*/
    struct ll_node_t* head; /**< pointer to the head of the list*/
    struct ll_node_t* tail; /**< pointer to the tail of the list*/
} linkedlist;

/**
 * Initializes a new empty linkedlist.
 * Remember to call ll_free when done!
 */
linkedlist* ll_initialize();

/**
 * Adds a new node to the end of a linkedlist.
 * 
 * @param list The list to append to.
 * @param value The value of the new node.
 */
void ll_push(linkedlist* list, int value);

/**
 * Gets the value of the node at an index.
 * 
 * @param list The list to search.
 * @param index The index of the node to be retrieved. Lists are zero-indexed.
 * @return The value of the node, or NULL if no such node exists.
 */
int ll_get(const linkedlist* list, int index);

/**
 * Frees all memory from the specified linkedlist.
 * If  `ptr` is NULL, no operation is performed.
 * 
 * @param ptr Pointer to linkedlist to be freed.
 */
void ll_free(linkedlist* ptr);

/**
 * Prints all values from the specified linkedlist.
 * 
 * @param list The list to print.
 */
void ll_print(const linkedlist* list);

#endif