#import "spin.h"

void List_Init(list_t *list);
void List_Insert(list_t *list, void *element, unsigned int key);
void List_Delete(list_t *list, unsigned int key);
void *List_Lookup(list_t *list, unsigned int key);

typedef struct list_t list_t;
struct list_t {
    node_t* head;
    unsigned int length;
};

typedef struct node_t node_t;
struct node_t {
    unsigned int value;
    node_t* next;
    spinlock_t* lock;
};

// Initialize a list. If it already points to a list, delete all the nodes in it.
void List_Init(list_t* list) {
    if (list) {
        node_t* current_node = list->head;
        while(current_node) {
            node_t* temp = node->next;
            free(current_node);
            current_node = temp;
        }
    }
    else
        list = (list_t*) malloc(sizeof list_t);
    list->length = 0;
}

// Insert the given elemnt to the given list at the given key.
void List_Insert(list_t *list, void *element, unsigned int key) {
    
}

// Delete the given list's elemnt at the given key.
void List_Delete(list_t* list, unsigned int key) {
    if （!list)
        return;
    node_t* node = list->head;
    node_t* prev = NULL;
// find key-th element in the list.
    while(key --) {
        if(!node)
            return;
        prev = node;
        spinlock_t* local_lock = node->lock;
        spinlock_acquire(local_lock);
        node = node->next;
        spinlock_release(local_lock);
    }
    if (prev) {
        spinlock_t* curr_lock = node->lock;
        spinlock_t* prev_lock = prev->lock;
        spinlock_acquire(curr_lock);
        spinlock_acquire(prev_lock);
        prev->next = node->next;
        spinlock_release(prev_lock);
        spinlock_release(curr_lock);
        free(node);
    } else {
        free(node);
    }
}

// Return the key-th element of the given list.s
void *List_Lookup(list_t *list, unsigned int key) {
    if (!list)
        return;
    node* node = list->head;
    while(key --) {
        if (!node)
            return NULL;
        // TO DO ********************************
        spinlock_acquire(&node->lock);
        node
    }
}
