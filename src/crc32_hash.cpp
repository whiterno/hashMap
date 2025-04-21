#include <stdint.h>

#include <stdio.h>

#include "crc32_hash.h"
#include "string_t.h"

static uint32_t bitsReverse(uint32_t num);
static uint32_t crc32_u32(uint32_t crc, uint32_t value);

uint32_t crc32HashString(string_t string){
    uint32_t hash      = 0;
    uint32_t chars     = 0;
    uint32_t crc       = 0x12345678;
    int remainder = 0;

    for (; remainder < int(string.length) - 3; remainder += 4){
        chars = *(uint32_t*)(string.string + remainder);
        hash += crc32_u32(crc, chars);
    }

    chars = 0;
    for (int i = string.length - 1; i >= remainder; i--){
        chars = chars * 16 + *(char*)(string.string + i);
    }
    hash += crc32_u32(crc, chars);

    return hash;
}


static uint32_t bitsReverse(uint32_t num){
    num = ((num & 0xAAAAAAAA) >> 1) | ((num & 0x55555555) << 1);
    num = ((num & 0xCCCCCCCC) >> 2) | ((num & 0x33333333) << 2);
    num = ((num & 0xF0F0F0F0) >> 4) | ((num & 0x0F0F0F0F) << 4);
    num = ((num & 0xFF00FF00) >> 8) | ((num & 0x00FF00FF) << 8);
    num = ((num & 0xFFFF0000) >> 16) | ((num & 0x0000FFFF) << 16);

    return num;
}

static uint32_t crc32_u32(uint32_t crc, uint32_t v){
    uint32_t tmp1 = bitsReverse(v);
    uint32_t tmp2 = bitsReverse(crc);

    uint64_t tmp5 = ((uint64_t)tmp1 << 32) ^ ((uint64_t)tmp2 << 32);

    uint64_t polynomial = 0x11EDC6F41ULL;

    for (int i = 0; i < 32; i++){
        if (tmp5 & (1ULL << (63 - i))){
            tmp5 ^= (polynomial << (31 - i));
        }
    }

    uint32_t result = bitsReverse((uint32_t)tmp5);

    return result;
}
