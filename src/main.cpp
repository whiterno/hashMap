#include <stdio.h>
#include <immintrin.h>

#include "tests.h"
#include "hash_funcs.h"

int main(int argc, char* argv[]){
    testAll("book.txt", atoi(argv[1]), argv[2]);
}
