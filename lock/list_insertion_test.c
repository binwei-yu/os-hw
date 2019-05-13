#include <stdio.h>
#include <pthread.h>
#include "list.h"

#define MAX_INSERTION 1000000
#define MAX_THREAD 20

list_t* list;

void* insert() {
    for (int i = 0; i < MAX_INSERTION; i ++) {
        List_Insert(list, "test", i);
    }
}

int main() {
    printf("threads, time(us), length\n");
    for (int i = 0; i <= MAX_THREAD; i ++) {
        list = (list_t*) malloc(sizeof (list_t));
        List_Init(list);
        pthread_t threads[i];
        struct timeval start;
        struct timeval end;
        unsigned long diff;
        gettimeofday(&start, NULL);
        for (int j = 0; j < i; j ++) {
            int temp = pthread_create(&threads[j], NULL, insert, NULL);
        }
        for (int j = 0; j < i; j ++) {
            pthread_join(threads[j], NULL);
        }
        gettimeofday(&end, NULL);
        diff = 1000000 * (end.tv_sec-start.tv_sec) + end.tv_usec-start.tv_usec;
        printf("%d, %ld, %d\n", i, diff, List_GetLength(list));
        List_Free(list);
    }
    return 1;
}
