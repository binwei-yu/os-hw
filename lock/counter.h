#ifndef COUNTER_H
#define COUNTER_H

#include "spin.h"

#define BUCKET_NUMBER 128

typedef struct bucket_t bucket_t;
typedef struct counter_t counter_t;
struct bucket_t {
    spinlock_t* lock;
    int count;
};

struct counter_t {
    bucket_t** buckets;
};

void Counter_Init(counter_t* c, int value);
int Counter_GetValue(counter_t* c);
void Counter_Increment(counter_t* c);
void Counter_Decrement(counter_t* c);
void Counter_Free(counter_t* c);

#endif
