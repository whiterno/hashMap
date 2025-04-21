#include <stdio.h>
#include <immintrin.h>

#include "crc32_hash.h"
#include "hash_map.h"
#include "list.h"
#include "utils.h"

int main(){
    size_t file_size = 0;
    char* text = readFile("book.txt", &file_size);

    uint32_t lines = countLines(text, file_size);

    string_t* string_array = buildStringArray(text, lines);

    HashMap hashMap = hashMapCtor(crc32HashString, BASE_HASH_MAP_CAPACITY);
    for (int i = 0; i < lines; i++){
        hashMapAddElement(&hashMap, string_array[i]);
    }

    dumpStatisticsToFile("crc32.txt", &hashMap);
}
