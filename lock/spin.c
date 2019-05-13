#import "spin.h"

#define MUTEX 1

void spinlock_init(spinlock_t* lock) {
#ifdef TEST
    lock->value = 0;
#else
    pthread_mutex_init(lock, NULL);
#endif
}

void spinlock_acquire(spinlock_t* lock) {
#ifdef TEST
    while(xchg(&lock->value, BUSY));
#else
    pthread_mutex_lock(lock);
#endif
}

void spinlock_release(spinlock_t* lock) {
#ifdef TEST
    lock->value = 0;
#else
    pthread_mutex_unlock(lock);
#endif
}

