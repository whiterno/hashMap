#include <stdio.h>
#include <immintrin.h>

#include "tests.h"
#include "hash_funcs.h"

int main(int argc, char* argv[]){
    if (argc == 2) testAll("book.txt", atoi(argv[1]));
}
