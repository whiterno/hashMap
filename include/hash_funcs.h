#ifndef HASH_FUNCS_H
#define HASH_FUNCS_H

#include <stdint.h>

#include "string_t.h"

uint32_t crc32HashString(string_t string);
uint32_t murmur3HashString(string_t string);

#endif
