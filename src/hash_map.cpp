#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <immintrin.h>
#include <stdalign.h>

#include "string_t.h"
#include "hash_funcs.h"
#include "hash_map.h"
#include "utils.h"
#include "list.h"

static uint32_t rehashAddElement(HashMap* hashMap, data_t data);

string_t* buildStringArray(char* text, uint32_t lines){
    assert(text);

    // string_t* string_array = (string_t*)calloc(lines, sizeof(string_t));
    string_t* string_array = (string_t*)aligned_alloc(sizeof(string_t), lines * sizeof(string_t));
    char* aligned_text     = (char*)aligned_alloc(YMM_BYTES_SIZE, YMM_BYTES_SIZE * lines);
    char* word_begin       = NULL;
    uint32_t length        = 0;

    assert(aligned_text);
    assert(((uintptr_t)aligned_text % 32) == 0);

    for (uint32_t i = 0; i < lines; i++){
        word_begin = text;

        text  = strchr(text, '\n');
        text++;

        length = text - word_begin;

        strncpy(aligned_text + 32 * i, word_begin, length);
        memset(aligned_text + 32 * i + length, '\0', 32 - length);

        #ifndef STRING_TO_VEC

            string_array[i].string = aligned_text + 32 * i;

        #else
            string_array[i].string = _mm256_load_si256((__m256i*)(aligned_text + 32 * i));
        #endif

        string_array[i].length = length;
    }

    #ifdef STRING_TO_VEC

        free(aligned_text);

    #endif

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

    #ifndef CRC32_OPTIMIZATION

        #ifndef REMAINDER_OPTIMIZATION

            uint32_t supposed_index = hashMap->hash_func(data) % hashMap->capacity;

        #else

            uint32_t supposed_index = hashMap->hash_func(data) & (hashMap->capacity - 1);

        #endif

    #else

        uint32_t hash  = 0;
        uint64_t crc   = 0x1212121121111111;

        #ifndef STRING_TO_VEC

            uint64_t hash1 = *(uint64_t*)(data.string);
            uint64_t hash2 = *(uint64_t*)(data.string + 8);
            uint64_t hash3 = *(uint64_t*)(data.string + 16);
            uint64_t hash4 = *(uint64_t*)(data.string + 24);

        #else

            uint64_t hash1 = _mm256_extract_epi64(data.string, 0);
            uint64_t hash2 = _mm256_extract_epi64(data.string, 1);
            uint64_t hash3 = _mm256_extract_epi64(data.string, 2);
            uint64_t hash4 = _mm256_extract_epi64(data.string, 3);

        #endif

        hash1 = _mm_crc32_u64(crc, hash1);
        hash2 = _mm_crc32_u64(crc, hash2);
        hash3 = _mm_crc32_u64(crc, hash3);
        hash4 = _mm_crc32_u64(crc, hash4);

        hash  = hash1 + hash2 + hash3 + hash4;

        #ifndef REMAINDER_OPTIMIZATION

            uint32_t supposed_index = hash % hashMap->capacity;

        #else

            uint32_t supposed_index = hash & (hashMap->capacity - 1);

        #endif

    #endif

    uint32_t inx = searchElement(&hashMap->lists[supposed_index], data);

    if (inx == 0) return false;

    return true;
}

float getLoadFactor(HashMap* hashMap){
    assert(hashMap);

    return hashMap->load_factor;
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

HashMap resize(HashMap* hashMap, uint32_t new_capacity){
    assert(hashMap);

    HashMap new_hashMap = hashMapCtor(hashMap->hash_func, new_capacity);
    new_hashMap.load_factor = hashMap->load_factor / (float)new_capacity * hashMap->capacity;

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
