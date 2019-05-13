#include <stdio.h>
#include <pthread.h>
#include "counter.h"

#define MAX_COUNT 1000000
#define MAX_THREAD  20
void* count();
counter_t* c;

void* count() {
    for(int i = 0; i < MAX_COUNT; i ++) {
        Counter_Increment(c);
    }
}


int main() {
    printf("threds, time(ms), counted\n");
    for (int i = 0; i <= MAX_THREAD; i ++) {
        c = (counter_t*) malloc(sizeof (counter_t));
        Counter_Init(c, 0);
        pthread_t threads[i];
        struct timeval start;
        struct timeval end;
        unsigned long diff;
        gettimeofday(&start, NULL);
        for (int j = 0; j < i; j ++) {
            int temp = pthread_create(&threads[j], NULL, count, NULL);
        }
        for (int j = 0; j < i; j ++) {
            pthread_join(threads[j], NULL);
        }
        gettimeofday(&end, NULL);
        diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
        printf("%d, %ld, %d\n", i, diff, Counter_GetValue(c));
        Counter_Free(c);
    }
    return 1;
}
