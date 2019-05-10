#import "spin.h"
#import <pthread.h>

int i = 0;
spinlock_t lock;

void sleep();
void* do_1();
void* do_2();

void* do_1() {
    spinlock_acquire(&lock);
    sleep(5);
    printf("do_1: %d\n", i ++);
    spinlock_release(&lock);
}

void* do_2() {
    spinlock_acquire(&lock);
    sleep(1);
    printf("do_2: %d\n", i ++);
    spinlock_release(&lock);
}




int main (){
    pthread_t t1;
    pthread_t t2;
    pthread_create(&t1, NULL, do_1, NULL);
    pthread_create(&t2, NULL, do_2, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}

