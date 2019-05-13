#ifndef HASH_H
#define HASH_H
#include "spin.h"
#include "list.h"

typedef struct hash_t {
    unsigned int size;
    list_t** buckets;
} hash_t;

void Hash_Init(hash_t *hash, int buckets);
void Hash_Insert(hash_t *hash, void *element, unsigned int key);
void Hash_Delete(hash_t *hash, unsigned int key);
void *Hash_Lookup(hash_t *hash, unsigned int key);
unsigned int h(unsigned int x);
void Hash_Free(hash_t* hash);
unsigned int Hash_GetSize(hash_t* hash);

#endif
