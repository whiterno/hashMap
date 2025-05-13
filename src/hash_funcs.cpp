#include <stdint.h>
#include <stdio.h>
#include <immintrin.h>

#include "hash_funcs.h"
#include "string_t.h"

static uint32_t bitsReverse(uint32_t num);
static uint32_t crc32_u32(uint32_t crc, uint32_t value);

uint32_t crc32HashString(string_t string){
    uint32_t hash  = 0;

    #ifndef STRING_TO_VEC

        uint32_t chars = 0000000000;
        uint32_t crc   = 0x11111111;
        int remainder  = 0;
        int length     = int(string.length);

        for (int i = 0; i < length - 1; i += 4){
            chars = *(uint32_t*)(string.string + i);
            hash += crc32_u32(crc, chars);
        }

    #endif

    return hash;
}

uint32_t _mm_crc32HashString(string_t string){
    uint32_t hash  = 0;

    #ifndef STRING_TO_VEC

        uint32_t chars = 0;
        uint32_t crc   = 0x11111111;
        int length     =  int(string.length);

        for (int i = 0; i < length - 1; i += 4){
            chars = *(uint32_t*)(string.string + i);
            hash += _mm_crc32_u32(crc, chars);
        }

    #endif

    return hash;
}

uint32_t _mm_crc32UnrollHashString(string_t string){
    uint32_t hash  = 0;
    uint64_t crc   = 0x1212121121111111;

    #ifndef STRING_TO_VEC

        uint64_t hash1 = *(uint64_t*)(string.string);
        uint64_t hash2 = *(uint64_t*)(string.string + 8);
        uint64_t hash3 = *(uint64_t*)(string.string + 16);
        uint64_t hash4 = *(uint64_t*)(string.string + 24);

    #else

        uint64_t hash1 = _mm256_extract_epi64(string.string, 0);
        uint64_t hash2 = _mm256_extract_epi64(string.string, 1);
        uint64_t hash3 = _mm256_extract_epi64(string.string, 2);
        uint64_t hash4 = _mm256_extract_epi64(string.string, 3);

    #endif

    hash1 = _mm_crc32_u64(crc, hash1);
    hash2 = _mm_crc32_u64(crc, hash2);
    hash3 = _mm_crc32_u64(crc, hash3);
    hash4 = _mm_crc32_u64(crc, hash4);

    hash  = hash1 + hash2 + hash3 + hash4;

    return hash;
}

uint32_t murmur2HashString(string_t string){

    #ifndef STRING_TO_VEC

        uint32_t num    = 0x5bd1e995;
        uint32_t seed   = 0;
        int32_t  shift  = 24;
        uint32_t length = string.length;
        unsigned char* data = (unsigned char*)string.string;

        uint32_t hash    = seed ^ length;
        uint32_t xor_num = 0;

        while (length >= 4)
        {
            xor_num  = data[0];
            xor_num |= data[1] << 8;
            xor_num |= data[2] << 16;
            xor_num |= data[3] << 24;

            xor_num *= num;
            xor_num ^= xor_num >> shift;
            xor_num *= num;

            hash *= num;
            hash ^= xor_num;

            data += 4;
            length -= 4;
        }

        switch (length)
        {
            case 3: hash ^= data[2] << 16;
            case 2: hash ^= data[1] << 8;
            case 1: hash ^= data[0];
                    hash *= num;
        };

        hash ^= hash >> 13;
        hash *= num;
        hash ^= hash >> 15;

    #else

        uint32_t hash = 0;

    #endif

    return hash;
}

uint32_t sumHashString(string_t string){
    uint32_t hash  = 0;

    #ifndef STRING_TO_VEC

        uint32_t chars = 0;
        int remainder  = 0;

        for (; remainder < int(string.length) - 3; remainder += 4){
            chars = *(uint32_t*)(string.string + remainder);
            hash += chars;
        }

        chars = 0;
        for (int i = remainder; i < string.length; i++){
            chars = chars * 256 + string.string[i];
        }
        for (int i = 4 - string.length % 4; i > 0; i--){
            chars *= 256;
        }
        if (chars != 0) hash += chars;

    #endif

    return hash;
}

uint32_t adler32HashString(string_t string){
    uint32_t A = 1;
    uint32_t B = 0;
    uint32_t mod_adler = 65521;

    for (int i = 0; i < string.length; i++)
    {
        A = (A + string.string[i]) % mod_adler;
        B = (B + A) % mod_adler;
    }

    return (B << 16) | A;
}

uint32_t elfHashString(string_t string){
    uint32_t hash = 0;
    uint32_t high = 0;
    for (int i = 0; i < string.length; i++){
        hash = (hash << 4) + string.string[i];

        if (high = hash & 0xF0000000) hash ^= high >> 24;

        hash &= ~high;
    }
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

    uint64_t tmp = ((uint64_t)tmp1 << 32) ^ ((uint64_t)tmp2 << 32);

    uint64_t polynomial = 0x11EDC6F41ULL;

    for (int i = 0; i < 32; i++){
        if (tmp & (1ULL << (63 - i))){
            tmp ^= (polynomial << (31 - i));
        }
    }

    uint32_t result = bitsReverse((uint32_t)tmp);

    return result;
}
