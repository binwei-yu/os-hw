#include "list.h"
#include <stdio.h>

list_t* list;

int main() {
    list = (list_t*) malloc(sizeof (list_t));
    List_Init(list);
    char** a = "abc";
    List_Insert(list, a, 0);
    List_Insert(list, a, 1);
    List_Insert(list, a, 2);
    List_Insert(list, a, 3);
    List_Insert(list, a, 4);
    printf("%s\n", List_Lookup(list, 0));
    printf("%s\n", List_Lookup(list, 1));
    printf("%s\n", List_Lookup(list, 2));
    printf("%s\n", List_Lookup(list, 3));
    printf("%s\n", List_Lookup(list, 4));
    List_Delete(list, 0);
    List_Delete(list, 1);
    List_Delete(list, 2);
    List_Delete(list, 3);
    List_Delete(list, 4);
    if(!(List_Lookup(list, 0)))
        printf("Doesn't exist!\n");
    if(!(List_Lookup(list, 1)))
        printf("Doesn't exist!\n");
    if(!(List_Lookup(list, 2)))
        printf("Doesn't exist!\n");
    if(!(List_Lookup(list, 3)))
        printf("Doesn't exist!\n");
    if(!(List_Lookup(list, 4)))
        printf("Doesn't exist!\n");
//    List_Insert(list, a, 0);
    List_Free(list);
    return 1;
}
