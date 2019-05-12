#include "list.h"
#include "hash.h"
#include <stdlib.h>

// Initialize a hash table
void Hash_Init(hash_t* hash, int buckets) {
    // Initialize buckets and size
    hash->size = buckets;
    hash->buckets = (list_t*)malloc(sizeof(list_t) * buckets);
    for(int i = 0; i < buckets; i++)
        List_Init(&hash->buckets[i]);
}

// Insert an element into a hash table
void Hash_Insert(hash_t* hash, void* element, unsigned int key) {
    unsigned int idx = h(key) % hash->size;
    List_Insert(&hash->buckets[idx], element, key);
}

// Delete an element from a hash table
void Hash_Delete(hash_t* hash, unsigned int key) {
    unsigned int idx = h(key) % hash->size;
    List_Delete(&hash->buckets[idx], key);
}

// Look up an element in a hash table
void* Hash_Lookup(hash_t* hash, unsigned int key) {
    unsigned int idx = h(key) % hash->size;
    return List_Lookup(&hash->buckets[idx], key);
}

// Hash function
unsigned int h(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}
