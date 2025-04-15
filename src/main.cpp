#include <stdio.h>
#include <immintrin.h>

#include "crc32_hash.h"
#include "hash_map.h"

int main(){
    char* text = readFile("book.txt");

    if (text) printf("%s", text);
    else printf("Can't open this file\n");
}
