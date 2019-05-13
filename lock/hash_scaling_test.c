#include <stdio.h>
#include <pthread.h>
#include "hash.h"
#include <math.h>

#define MIN_POWER 0
#define MAX_POWER 5
#define BUCKETS_STEP 10
#define THREAD_NUM 20
#define MAX_INSERTION 1000000

hash_t* hash;

void* insert() {
    for (int i = 0; i < MAX_INSERTION; i ++) {
        Hash_Insert(hash, "test", i);
    }
}

int main() {
    printf("buckets, time(us), size\n");
    for (int i = MIN_POWER; i <= MAX_POWER; i ++) {
        hash = (hash_t*) malloc(sizeof (hash_t));
        Hash_Init(hash, pow(10, i));
        pthread_t threads[THREAD_NUM];
        struct timeval start;
        struct timeval end;
        unsigned int diff;
        gettimeofday(&start, NULL);
        for (int j = 0; j < THREAD_NUM; j ++) {
            int temp = pthread_create(&threads[j], NULL, insert, NULL);
        }
        for (int j = 0; j < THREAD_NUM; j ++) {
            pthread_join(threads[j], NULL);
        }
        gettimeofday(&end, NULL);
        diff = 1000000 * (end.tv_sec-start.tv_sec) + end.tv_usec-start.tv_usec;
        printf("%d, %ld, %d\n", i, diff, Hash_GetSize(hash));
        Hash_Free(hash);
    }
}
