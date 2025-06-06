#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdint.h>

#include "string_t.h"

#ifndef RESIZABLE_USER

const int LOAD_FACTOR = 15;

#endif

const int YMM_BYTES_SIZE = 32;

typedef string_t data_t;
typedef uint32_t (*hash_t)(data_t data);

typedef struct ListElem{
    uint32_t next_inx;
    uint32_t prev_inx;

    uint32_t counter;

    #ifdef STRING_TO_VEC
    char dummy[52];
    #endif

    data_t data;
} ListElem;

typedef struct List{
    ListElem* list_elems;
    uint32_t tail_inx;
    uint32_t capacity;
    uint32_t elements_amount;
} List;

typedef struct HashMap{
    List* lists;
    uint32_t capacity;
    float load_factor;
    hash_t hash_func;
} HashMap;

string_t* buildStringArray(char* text, uint32_t lines);
HashMap hashMapCtor(hash_t hash_func, uint32_t capacity);
uint32_t hashMapAddElement(HashMap* hashMap, data_t data);
uint32_t hashMapPopElement(HashMap* hashMap, data_t data);
HashMap resize(HashMap* hashMap, uint32_t new_capacity);
bool hashMapSearchElement(HashMap* hashMap, data_t data);
void hashMapDtor(HashMap* hashMap);

#endif
