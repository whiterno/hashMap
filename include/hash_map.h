#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdint.h>

#include "string_t.h"

const int LOAD_FACTOR = 15;

typedef struct HashMapElem{
    String_t string;
    uint32_t hash;
} HashMapElem;

typedef struct HashMapCell{
    HashMapElem* elems;
    uint32_t length;
} HashMapCell;

typedef struct HashMap{
    HashMapCell* cells;
    uint32_t length;
} HashMap;

char* readFile(const char* filename);

#endif
