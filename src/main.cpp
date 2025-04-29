#include <stdio.h>
#include <immintrin.h>

#include "tests.h"
#include "hash_funcs.h"
#include "hash_map.h"

int main(int argc, char* argv[]){

    uint32_t test_amount =  atoi(argv[1]);
    char* test_name = argv[2];

    // 1024 is used for this project, but you can change it if you need
    uint32_t capacity = 1024;

    testAll("book.txt", test_amount, test_name, capacity);
}
