#include <stdio.h>
#include <immintrin.h>

#include "tests.h"
#include "hash_funcs.h"

int main(int argc, char* argv[]){
    if (argc == 2) testAll("book.txt", atoi(argv[1]));

    // char word1[6] = "hello";
    // char word2[6] = "world";
    // char word3[3] = "no";

    // string_t str1 = {.string = word1, .hash = 0, .length = 6};
    // string_t str2 = {.string = word2, .hash = 0, .length = 6};
    // string_t str3 = {.string = word3, .hash = 0, .length = 3};

    // printf("MY CRC32: %u\n", crc32HashString(str3));
    // printf("CRC32:    %u\n", _mm_crc32_u32(0x12345678, 0x6e6f0000));
}
