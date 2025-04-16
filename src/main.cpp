#include <stdio.h>
#include <immintrin.h>

#include "crc32_hash.h"
#include "hash_map.h"
#include "list.h"

int main(){
    size_t file_size = 0;
    char* text = readFile("book.txt", &file_size);

    // char word1[6] = "hello";
    // char word2[6] = "world";
    // char word3[3] = "no";

    // string_t str1 = {.string = word1, .hash = 0, .length = 6};
    // string_t str2 = {.string = word2, .hash = 0, .length = 6};
    // string_t str3 = {.string = word3, .hash = 0, .length = 3};

    // List list = listCtor();
    // debugPrint(&list);
    // pushListElem(&list, str1);
    // debugPrint(&list);
    // pushListElem(&list, str2);
    // debugPrint(&list);
    // pushListElem(&list, str3);
    // debugPrint(&list);
    // popListElem(&list, str2);
    // debugPrint(&list);
}
