#ifndef TESTS_H
#define TESTS_H

#include "hash_map.h"

const uint32_t SEARCH_ELEMS_AMOUNT = 1000000;

typedef struct TestData{
    data_t* data_array;
    uint32_t lines;
} TestData;

typedef struct Filenames{
    const char* collision_filename;
    const char* search_filename;
} Filenames;


#define TEST(hash_name){                                                                                        \
    printf("\n==============================\n");                                                               \
    printf(#hash_name "\n");                                                                                    \
    printf("------------------------------\n");                                                                 \
    Filenames hash_name ## _files = {.collision_filename = "plot/" #hash_name "/" #hash_name "_collisions.csv", \
                                     .search_filename =  "plot/" #hash_name "/" #hash_name "_search_time.csv"}; \
    test(hash_name ## HashString, hash_name ## _files, test_data, test_amount);                                 \
    printf("==============================\n");                                                                 \
}

void testAll(const char* filename, uint32_t test_amount, const char* test_name);
void test(hash_t hash_func, Filenames filenames, TestData test_data, uint32_t test_amount);
void testSearchTime(FILE* file, hash_t hash_func, TestData test_data, uint32_t test_amount);
void testCollisions(FILE* file, hash_t hash_func, TestData test_data);
double standartDeviationTime(double* time_array, uint32_t length, double average);
float standartDeviationCollisions(HashMap* hashMap, float average);

#endif
