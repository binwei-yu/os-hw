#include "spin.h"
#include <pthread.h>
#define BUCKET_NUMBER 128

typedef struct counter_t counter_t;
typedef struct bucket_t bucket_t;

void Counter_Init(counter_t *c, int value);
int Counter_GetValue(counter_t *c);
void Counter_Increment(counter_t *c);
void Counter_Decrement(counter_t *c);


struct bucket_t {
    spinlock_t* lock;
    int count;
};
struct counter_t {
    bucket_t** buckets;
};


// Initialize a given counter to a given value when c is NULL
void Counter_Init(counter_t* c, int value) {
    if (!c) return;
    c->buckets = (bucket_t**) malloc(BUCKET_NUMBER * sizeof(bucket_t*));
    for (int i = 0; i < BUCKET_NUMBER; i ++) {
        c->buckets[i] = (bucket_t*) malloc(sizeof(bucket_t));
        c->buckets[i]->lock = (spinlock_t*) malloc(sizeof(spinlock_t));
        c->buckets[i]->count = i == 0 ? value : 0;
    }
}

// Get the current value of a given counter.
int Counter_GetValue(counter_t* c) {
    if (!c) return 0;
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
    int index = (int) pthread_self() % BUCKET_NUMBER;
    spinlock_acquire(c->buckets[index]->lock);
    c->buckets[index]->count ++;
    spinlock_release(c->buckets[index]->lock);
}

// Decrease a given counter by 1.
void Counter_Decrement(counter_t* c) {
    if (!c) return;
    int index = (int) pthread_self() % BUCKET_NUMBER;
    spinlock_acquire(c->buckets[index]->lock);
    c->buckets[index]->count --;
    spinlock_release(c->buckets[index]->lock);
}
