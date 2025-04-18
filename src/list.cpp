#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hash_map.h"
#include "list.h"

static void resizeUp(List* list);
static int cmpListElems(data_t* data1, data_t* data2);

List listCtor(){
    List list = {.list_elems = NULL, .capacity = LIST_BASE_CAPACITY};
    list.list_elems = (ListElem*)calloc(LIST_BASE_CAPACITY, sizeof(ListElem));
    list.list_elems[0].next_inx = 0;
    list.list_elems[0].prev_inx = 0;

    list.tail_inx = 1;
    for (uint32_t i = 1; i < LIST_BASE_CAPACITY; i++){
        list.list_elems[i].next_inx = i + 1;
        list.list_elems[i].prev_inx = i;
    }

    return list;
}

int pushListElem(List* list, data_t data){
    assert(list);

    uint32_t inx = searchElement(list, data);

    if (inx == 0){
        inx            = list->tail_inx;
        list->tail_inx = list->list_elems[inx].next_inx;

        list->list_elems[inx].data    = data;
        list->list_elems[inx].counter = 0;
        uint32_t old_last_inx         = list->list_elems[0].prev_inx;

        list->list_elems[inx].next_inx = 0;
        list->list_elems[inx].prev_inx = old_last_inx;

        list->list_elems[0].prev_inx            = inx;
        list->list_elems[old_last_inx].next_inx = inx;
    }

    list->list_elems[inx].counter++;

    if (list->tail_inx == list->capacity) resizeUp(list);

    return inx;
}

int popListElem(List* list, data_t data){
    assert(list);

    uint32_t inx = searchElement(list, data);

    if (inx == 0) return 0;

    uint32_t prev_inx = list->list_elems[inx].prev_inx;
    uint32_t next_inx = list->list_elems[inx].next_inx;

    list->list_elems[prev_inx].next_inx = next_inx;
    list->list_elems[next_inx].prev_inx = prev_inx;

    list->list_elems[inx].prev_inx = inx;
    list->list_elems[inx].next_inx = list->tail_inx;
    list->tail_inx = inx;

    return inx;
}

int searchElement(List* list, data_t data){
    assert(list);

    uint32_t inx = list->list_elems[0].next_inx;

    while (inx != 0){
        if (cmpListElems(&data, &list->list_elems[inx].data) == 0) return inx;

        inx = list->list_elems[inx].next_inx;
    }

    return inx;
}

static void resizeUp(List* list){
    list->capacity  *= 2;
    list->list_elems = (ListElem*)realloc(list->list_elems, list->capacity * sizeof(ListElem));
    for (uint32_t i = list->tail_inx; i < list->capacity; i++){
        list->list_elems[i].next_inx = i + 1;
        list->list_elems[i].prev_inx = i;
    }
}

static int cmpListElems(data_t* data1, data_t* data2){
    assert(data1);
    assert(data2);

    return strcmp(data1->string, data2->string);
}

void debugPrint(List* list){
    assert(list);

    printf("CAPACITY: %u\n", list->capacity);
    printf("TAIL_INX: %u\n", list->tail_inx);
    printf("INXS:     ");
    for (int i = 0; i < list->capacity; i++){
        printf("%u ", i);
    }
    printf("\n");
    printf("NEXT:     ");
    for (int i = 0; i < list->capacity; i++){
        printf("%u ", list->list_elems[i].next_inx);
    }
    printf("\n");
    printf("PREV:     ");
    for (int i = 0; i < list->capacity; i++){
        printf("%u ", list->list_elems[i].prev_inx);
    }
    printf("\n");
}
