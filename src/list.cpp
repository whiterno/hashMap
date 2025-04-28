#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdalign.h>

#include "hash_map.h"
#include "list.h"

static void resizeUp(List* list);
static int cmpListElems(data_t* data1, data_t* data2);

List listCtor(){
    List list = {.list_elems = NULL, .capacity = LIST_BASE_CAPACITY};
    list.list_elems = (ListElem*)calloc(LIST_BASE_CAPACITY, sizeof(ListElem));
    list.list_elems[0].next_inx = 0;
    list.list_elems[0].prev_inx = 0;
    list.elements_amount = 0;

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

        list->elements_amount++;
    }

    list->list_elems[inx].counter++;

    if (list->tail_inx == list->capacity) resizeUp(list);

    return list->list_elems[inx].counter;
}

int popListElem(List* list, data_t data){
    assert(list);

    uint32_t inx = searchElement(list, data);

    if (inx == 0) return 0;

    list->list_elems[inx].data.hash = 0;
    list->list_elems[inx].data.string = NULL;

    uint32_t prev_inx = list->list_elems[inx].prev_inx;
    uint32_t next_inx = list->list_elems[inx].next_inx;

    list->list_elems[prev_inx].next_inx = next_inx;
    list->list_elems[next_inx].prev_inx = prev_inx;

    list->list_elems[inx].prev_inx = inx;
    list->list_elems[inx].next_inx = list->tail_inx;
    list->tail_inx = inx;

    list->elements_amount--;

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
    memset(list->list_elems + list->capacity / 2, '\0', list->capacity / 2 * sizeof(ListElem));
    for (uint32_t i = list->tail_inx; i < list->capacity; i++){
        list->list_elems[i].next_inx = i + 1;
        list->list_elems[i].prev_inx = i;
    }
}

extern "C" {
    int my_strcmp(const char* str1, const char* str2);
}

static int cmpListElems(data_t* data1, data_t* data2){
    #ifdef STRCMP_OPTIMIZATION_INLINE
    uint32_t result = 0;

    asm volatile (
        "vmovdqa (%1), %%ymm0\n\t"
        "vmovdqa (%2), %%ymm1\n\t"
        "vpcmpeqb %%ymm1, %%ymm0, %%ymm2\n\t"
        "vpmovmskb %%ymm2, %0\n\t"
        "not %0\n\t"
        : "=r"(result)
        : "r"(data1->string), "r"(data2->string)
        : "ymm0", "ymm1", "ymm2"
    );

    return result;

    #endif

    #ifdef STRCMP_OPTIMIZATION

    return my_strcmp(data1->string, data2->string);

    #else

    return strcmp(data1->string, data2->string);

    #endif
}

void listDtor(List* list){
    assert(list);

    free(list->list_elems);

    list->capacity = 0;
    list->tail_inx = 0;
}

void debugPrint(List* list){
    assert(list);

    printf("CAPACITY: %u\n", list->capacity);
    printf("TAIL_INX: %u\n", list->tail_inx);
    printf("ELEM_AM : %u\n", list->elements_amount);
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
    printf("HASH:     ");
    for (int i = 0; i < list->capacity; i++){
        printf("%u ", list->list_elems[i].data.hash);
    }
    printf("\n");
    printf("STR:\n");
    for (int i = 0; i < list->capacity; i++){
        printf("%-32s\n", list->list_elems[i].data.string);
    }
    printf("\n");
}

