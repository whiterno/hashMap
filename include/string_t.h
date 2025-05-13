#ifndef STRING_T_H
#define STRING_T_H

#include <stdint.h>
#include <immintrin.h>

#ifndef STRING_TO_VEC

typedef struct string_t{
    char* string;
    uint32_t hash;
    uint32_t length;
} string_t;

#else

typedef struct string_t{
    __m256i string;
    uint32_t hash;
    uint32_t length;
    char dummy[24];
} string_t;

#endif

#endif
