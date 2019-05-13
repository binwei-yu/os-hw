#ifndef LIST_H
#define LIST_H
#include "spin.h"

typedef struct node_t node_t;
struct node_t {
    unsigned int key;
    void* element;
    node_t* next;
    spinlock_t* lock;
};
typedef struct list_t list_t;
struct list_t {
    spinlock_t* lock;
    node_t* head;
    unsigned int length;
};

void List_Init(list_t* list);
void List_Insert(list_t* list, void *element, unsigned int key);
void List_Delete(list_t* list, unsigned int key);
void *List_Lookup(list_t* list, unsigned int key);
void List_Free(list_t* list);
unsigned int List_GetLength(list_t* list);

#endif
