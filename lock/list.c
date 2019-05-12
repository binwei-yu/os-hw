#include "list.h"
#include <stdio.h>

list_t* list;

int main() {
    list = (list_t*) malloc(sizeof (list_t));
    List_Init(list);
    char** a = "abc";
    List_Insert(list, a, 0);
    List_Insert(list, a, 1);
    printf("%s\n", List_Lookup(list, 0));
    printf("%s\n", List_Lookup(list, 1));
    List_Delete(list, 1);
    if(!(List_Lookup(list, 1)))
        printf("Doesn't exist!\n");
    List_Free(list);
    return 1;
}
