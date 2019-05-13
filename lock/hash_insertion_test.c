#include <stdio.h>
#include <pthread.h>
#include "hash.h"

#define MAX_INSERTION 1000000
#define MAX_THREAD 20
#define BUCKET_NUM 50

hash_t* hash;

void* insert() {
    for (int i = 0; i < MAX_INSERTION; i ++) {
        Hash_Insert(hash, "test", i);
    }
}

int main() {
    printf("threads, time(us), size\n");
    for (int i = 0; i <= MAX_THREAD; i ++) {
        hash = (hash_t*) malloc(sizeof (hash_t));
        Hash_Init(hash, BUCKET_NUM);
        pthread_t threads[i];
        struct timeval start;
        struct timeval end;
        unsigned int diff;
        gettimeofday(&start, NULL);
        for (int j = 0; j < i; j ++) {
            int temp = pthread_create(&threads[j], NULL, insert, NULL);
        }
        for (int j = 0; j < i; j ++) {
            pthread_join(threads[j], NULL);
        }
        gettimeofday(&end, NULL);
        diff = 1000000 * (end.tv_sec-start.tv_sec) + end.tv_usec-start.tv_usec;
        printf("%d, %ld, %d\n", i, diff, Hash_GetSize(hash));
        Hash_Free(hash);
    }
}
