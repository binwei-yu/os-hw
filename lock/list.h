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
};

void List_Init(list_t* list);
void List_Insert(list_t* list, void *element, unsigned int key);
void List_Delete(list_t* list, unsigned int key);
void *List_Lookup(list_t* list, unsigned int key);
void List_Free(list_t* list);

// Initialize a list. If it already points to a list, delete all the nodes in it.
void List_Init(list_t* list) {
    if (!list) return;
    list->lock = (spinlock_t*) malloc(sizeof (spinlock_t));
    list->head = NULL;
}

// Insert the given elemnt to the given list at the given key.
void List_Insert(list_t *list, void *element, unsigned int key) {
    node_t* new_node = (node_t*) malloc(sizeof (node_t));
    new_node->key = key;
    new_node->element = element;
    new_node->next = NULL;
    new_node->lock = (spinlock_t*) malloc(sizeof (spinlock_t));
    
    spinlock_acquire(list->lock);
    new_node->next = list->head;
    list->head = new_node;
    spinlock_release(list->lock);
}

// Delete the given list's elemnt at the given key.
void List_Delete(list_t* list, unsigned int key) {
    if (!list || !list->head) return;
    // If head is a match
    spinlock_acquire(list->lock);
    node_t* previous_node = NULL;
    node_t* current_node = list->head;
    if (current_node->key == key) {
        list->head = list->head->next;
        free(current_node->lock);
        free(current_node);
        spinlock_release(list->lock);
        
        return;
    }
    spinlock_release(list->lock);
    
    // Not deleting the head
    spinlock_acquire(current_node->lock);
    previous_node = current_node;
    current_node = current_node->next;
    
    while(current_node) {
        spinlock_acquire(current_node->lock);
        if(current_node->key == key) {
            previous_node->next = current_node->next;
            spinlock_release(current_node->lock);
            spinlock_release(previous_node->lock);
            free(current_node->lock);
            free(current_node);
            
            return;
        }
        else {
            spinlock_release(previous_node->lock);
            previous_node = current_node;
            current_node = current_node->next;
        }
    }
}

// Return the key-th element of the given list.s
void* List_Lookup(list_t *list, unsigned int key) {
    if (!list) return NULL;
    spinlock_acquire(list->lock);
    node_t* current_node = list->head;
    spinlock_release(list->lock);
    while(current_node) {
        spinlock_acquire(current_node->lock);
        if (current_node->key == key) {
            spinlock_release(current_node->lock);
            return current_node->element;
        }
        spinlock_release(current_node->lock);
        current_node = current_node->next;
    }
    return NULL;
}

void List_Free(list_t* list) {
    spinlock_acquire(list->lock);
    node_t* current_node = list->head;
    while(current_node) {
        node_t* temp = current_node->next;
        free(current_node->lock);
        free(current_node);
        current_node = temp;
    }
    spinlock_release(list->lock);
    free(list);
}
