#import "spin.h"

void spinlock_acquire(spinlock_t* lock) {
    while(xchg(&lock->value, BUSY));
}

void spinlock_release(spinlock_t* lock) {
    lock->value = 0;
}

