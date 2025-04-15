#ifndef STRING_T_H
#define STRING_T_H

#include <stdint.h>

typedef struct String_t{
    char* string;
    uint32_t length;
} String_t;

#endif
