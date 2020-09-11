#include <stdlib.h>

// node type for linkedlist_t
typedef struct ll_node_t {
    int value;
    struct ll_node_t* next;
} node;

// Implementation of heap-allocated circular singly-linked list with head and tail pointers
// order is not guaranteed when multiple nodes have the same value
typedef struct linkedlist_t {
    int size;
    struct ll_node_t* head;
    struct ll_node_t* tail;
} linkedlist;

// initializes empty list
// remember to call ll_free when done!
linkedlist* ll_initialize();

// adds new node with value at end
void ll_push(linkedlist* list, int value);

// gets value of node at index
// DANGER: returns NULL if no such node exists
int ll_get(linkedlist* list, int index);

// free all memory from the specified linkedlist
// if ptr is null, no operation is performed
void ll_free(linkedlist* ptr);

// print out all values in list 
void ll_print(linkedlist* list);