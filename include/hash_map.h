#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdint.h>

#include "string_t.h"

const int LOAD_FACTOR = 15;

typedef string_t data_t;

typedef struct ListElem{
    uint32_t next_inx;
    uint32_t prev_inx;

    uint32_t counter;

    data_t data;
} ListElem;

typedef struct List{
    ListElem* list_elems;
    uint32_t tail_inx;
    uint32_t capacity;
} List;

typedef struct HashMap{
    List* lists;
    uint32_t capacity;
} HashMap;

char* readFile(const char* filename, size_t* size);

#endif
