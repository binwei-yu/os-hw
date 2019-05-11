#include <stdio.h>
#include "counter.h"

void* count();
counter_t* c;
int number = 0;

void* count() {
    for(int i = 0; i < 10000; i ++) {
        Counter_Increment(c);
        number ++;
    }
}

int main() {
    c = (counter_t*) malloc(sizeof (counter_t));
    Counter_Init(c, 0);
    pthread_t t1;
    pthread_t t2;
    pthread_create(&t1, NULL, count, NULL);
    pthread_create(&t2, NULL, count, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("%d\n", Counter_GetValue(c));
    printf("%d\n", number);
}
