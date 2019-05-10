import "spin.h"


typedef struct counter_t counter_t;
void Counter_Init(counter_t *c, int value);
int Counter_GetValue(counter_t *c);
void Counter_Increment(counter_t *c);
void Counter_Decrement(counter_t *c);


struct counter_t {
    unsigned int count;
    spinlock_t lock;
};

// Initialize a given counter to a given value.
void Counter_Init(counter_t* c, int value) {
    spinlock_acquire(&lock);
    c->count = value;
    spinlock_release(&lock);
}

// Get the current value of a given counter.
int Counter_GetValue(counter_t* c) {
    spinlock_acquire(&lock);
    int result = c->count
    spinlock_release(&lock);
    return result;
}

// Increase a given counter by 1.
void Counter_Increment(counter* c) {
    spinlock_acquire(&lock);
    c->coun ++;
    spinlock_release(&lock);
}

// Decrease a given counter by 1.
void Counter_Increment(counter* c) {
    spinlock_acquire(&lock);
    c->coun --;
    spinlock_release(&lock);
}
