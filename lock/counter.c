#include "counter.h"

unsigned int hash();

// Initialize a given counter to a given value when c is NULL
void Counter_Init(counter_t* c, int value) {
    if (!c) return;
    c->buckets = (bucket_t**) malloc(BUCKET_NUMBER * sizeof (bucket_t*));
    for (int i = 0; i < BUCKET_NUMBER; i ++) {
        c->buckets[i] = (bucket_t*) malloc(sizeof (bucket_t));
        c->buckets[i]->lock = (spinlock_t*) malloc(sizeof (spinlock_t));
        spinlock_init(c->buckets[i]->lock);
        c->buckets[i]->count = i == 0 ? value : 0;
    }
}

// Get the current value of a given counter.
int Counter_GetValue(counter_t* c) {
    if (!c) return NULL;
    int result = 0;
    for (int i = 0; i < BUCKET_NUMBER; i ++) {
        spinlock_acquire(c->buckets[i]->lock);
        result += c->buckets[i]->count;
        spinlock_release(c->buckets[i]->lock);
    }
    return result;
}

// Increase a given counter by 1.
void Counter_Increment(counter_t* c) {
    if (!c) return;
    unsigned int index = hash() % BUCKET_NUMBER;
    spinlock_acquire(c->buckets[index]->lock);
    c->buckets[index]->count ++;
    spinlock_release(c->buckets[index]->lock);
}

// Decrease a given counter by 1.
void Counter_Decrement(counter_t* c) {
    if (!c) return;
    unsigned int index = hash() % BUCKET_NUMBER;
    spinlock_acquire(c->buckets[index]->lock);
    c->buckets[index]->count --;
    spinlock_release(c->buckets[index]->lock);
}

// Free a counter
void Counter_Free(counter_t* c) {
    if (!c) return;
    for (int i = 0; i < BUCKET_NUMBER; i ++) {
        free(c->buckets[i]->lock);
        free(c->buckets[i]);
    }
    free(c->buckets);
    free(c);
}

// Hash Function
unsigned int hash() {
    unsigned int x = pthread_self();
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}
