#ifndef STRING_T_H
#define STRING_T_H

#include <stdint.h>
#include <immintrin.h>

typedef struct string_t{
    char* string;
    uint32_t hash;
    uint32_t length;
} string_t;

#endif
