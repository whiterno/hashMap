#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <immintrin.h>

#include "string_t.h"
#include "hash_funcs.h"
#include "hash_map.h"
#include "utils.h"
#include "list.h"

static uint32_t rehashAddElement(HashMap* hashMap, data_t data);
static HashMap rehash(HashMap* hashMap);

string_t* buildStringArray(char* text, uint32_t lines){
    assert(text);

    string_t* string_array = (string_t*)calloc(lines, sizeof(string_t));
    char* aligned_text     = (char*)aligned_alloc(YMM_BYTES_SIZE, YMM_BYTES_SIZE * lines);
    char* word_begin       = NULL;
    uint32_t length        = 0;

    for (uint32_t i = 0; i < lines; i++){
        word_begin = text;

        text  = strchr(text, '\n');
        text++;

        length = text - word_begin;

        strncpy(aligned_text + 32 * i, word_begin, length);
        memset(aligned_text + 32 * i + length, '\0', 32 - length);

        string_array[i].string = aligned_text + 32 * i;
        string_array[i].length = length;
    }

    return string_array;
}

HashMap hashMapCtor(hash_t hash_func, uint32_t capacity){
    HashMap hashMap = {.lists = NULL,
                       .capacity = capacity,
                       .load_factor = 0,
                       .hash_func = hash_func};

    hashMap.lists = (List*)calloc(capacity, sizeof(List));

    for (uint32_t i = 0; i < capacity; i++){
        hashMap.lists[i] = listCtor();
    }

    return hashMap;
}

uint32_t hashMapAddElement(HashMap* hashMap, data_t data){
    assert(hashMap);

    data.hash = hashMap->hash_func(data);
    uint32_t inx = data.hash % hashMap->capacity;

    uint32_t counter = pushListElem(hashMap->lists + inx, data);

    if (counter == 1)
        hashMap->load_factor = (hashMap->load_factor * hashMap->capacity + 1) / hashMap->capacity;

    if (hashMap->load_factor >= LOAD_FACTOR)
        *hashMap = rehash(hashMap);

    return inx;
}

uint32_t hashMapPopElement(HashMap* hashMap, data_t data){
    assert(hashMap);

    data.hash = hashMap->hash_func(data);
    uint32_t inx = data.hash % hashMap->capacity;

    if (popListElem(hashMap->lists + inx, data) == 0) return hashMap->capacity;

    hashMap->load_factor = (hashMap->load_factor * hashMap->capacity - 1) / hashMap->capacity;

    return inx;
}

bool hashMapSearchElement(HashMap* hashMap, data_t data){
    assert(hashMap);

    uint32_t supposed_index = hashMap->hash_func(data) % hashMap->capacity;
    uint32_t inx = searchElement(&hashMap->lists[supposed_index], data);

    if (inx == 0) return false;

    return true;
}

void hashMapDtor(HashMap* hashMap){
    assert(hashMap);

    for (uint32_t i = 0; i < hashMap->capacity; i++){
        listDtor(hashMap->lists + i);
    }

    free(hashMap->lists);
}

static uint32_t rehashAddElement(HashMap* hashMap, data_t data){
    assert(hashMap);

    uint32_t inx = data.hash % hashMap->capacity;

    uint32_t counter = pushListElem(hashMap->lists + inx, data);

    return inx;
}

static HashMap rehash(HashMap* hashMap){
    assert(hashMap);

    HashMap new_hashMap = hashMapCtor(hashMap->hash_func, hashMap->capacity * 2);
    new_hashMap.load_factor = hashMap->load_factor / 2;

    for (uint32_t i = 0; i < hashMap->capacity; i++){
        uint32_t elem_inx = (hashMap->lists + i)->list_elems[0].next_inx;

        while (elem_inx != 0){
            data_t data = (hashMap->lists + i)->list_elems[elem_inx].data;
            rehashAddElement(&new_hashMap, data);

            elem_inx = (hashMap->lists + i)->list_elems[elem_inx].next_inx;
        }
    }

    hashMapDtor(hashMap);

    return new_hashMap;
}

void hashMapDebugPrint(HashMap* hashMap){
    printf("==========================\n");

    if (hashMap == NULL){
        printf("NULL\n");
        printf("==========================\n");

        return;
    }

    printf("HM CAPACITY: %d\n", hashMap->capacity);
    printf("LOAD FACTOR: %f\n", hashMap->load_factor);

    for (uint32_t i = 0; i < hashMap->capacity; i++){
        printf("HASHMAP INX: %d\n", i);
        debugPrint(hashMap->lists + i);
    }
    printf("==========================\n");
}
