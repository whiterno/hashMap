#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>
#include <random>
#include <chrono>

#include "hash_funcs.h"
#include "hash_map.h"
#include "list.h"
#include "utils.h"
#include "tests.h"

void testAll(const char* filename, uint32_t test_amount, const char* test_name, uint32_t capacity){
    assert(filename);
    assert(test_name);

    size_t file_size = 0;
    char* text = readFile("book.txt", &file_size);

    uint32_t lines = countLines(text, file_size);

    TestData test_data = {.data_array = buildStringArray(text, lines),
                          .lines = lines};

    if (strcmp(test_name, "crc32") == 0)           TEST(crc32);
    if (strcmp(test_name, "_mm_crc32") == 0)       TEST(_mm_crc32);
    if (strcmp(test_name, "murmur3") == 0)         TEST(murmur2);
    if (strcmp(test_name, "sum") == 0)             TEST(sum);
    if (strcmp(test_name, "adler32") == 0)         TEST(adler32);
    if (strcmp(test_name, "elf") == 0)             TEST(elf);
    if (strcmp(test_name, "_mm_crc32Unroll") == 0) TEST(_mm_crc32Unroll);
    if (strcmp(test_name, "all") == 0){
        TEST(crc32);
        TEST(_mm_crc32);
        TEST(_mm_crc32Unroll);
        TEST(murmur2);
        TEST(sum);
        TEST(adler32);
        TEST(elf);
    }

    free(text);
    free(test_data.data_array[0].string);
    free(test_data.data_array);
}

void test(hash_t hash_func, Filenames filenames, TestData test_data, uint32_t test_amount, uint32_t capacity){
    #ifndef SEARCH_ONLY

    FILE* collison_file = fopen(filenames.collision_filename, "w");
    testCollisions(collison_file, hash_func, test_data, capacity);
    fclose(collison_file);

    printf("------------------------------\n");

    #endif

    FILE* search_file = fopen(filenames.search_filename, "w");
    testSearchTime(search_file, hash_func, test_data, test_amount, capacity);
    fclose(search_file);
}

void testCollisions(FILE* file, hash_t hash_func, TestData test_data, uint32_t capacity){
    assert(file);

    HashMap hashMap = hashMapCtor(hash_func, capacity);

    for (int j = 0; j < test_data.lines; j++){
        hashMapAddElement(&hashMap, test_data.data_array[j]);
    }

    float average_collision = 0;

    fprintf(file, "Bucket index,Bucket size\n");

    for (uint32_t i = 0; i < hashMap.capacity; i++){
        fprintf(file, "%u,%u\n", i, hashMap.lists[i].elements_amount);
        average_collision += hashMap.lists[i].elements_amount;
    }

    average_collision /= hashMap.capacity;

    printf("AVERAGE COLLISIONS: %.2f\n", average_collision);
    printf("STANDART DEVIATION: %.2f\n", standartDeviationCollisions(&hashMap, average_collision));

    hashMapDtor(&hashMap);
}

void testSearchTime(FILE* file, hash_t hash_func, TestData test_data, uint32_t test_amount, uint32_t capacity){
    assert(file);
    srand(10);

    HashMap hashMap = hashMapCtor(hash_func, capacity);
    for (int j = 0; j < test_data.lines; j++){
        hashMapAddElement(&hashMap, test_data.data_array[j]);
    }

    uint32_t* index_array = (uint32_t*)calloc(SEARCH_ELEMS_AMOUNT, sizeof(uint32_t));
    double* search_time_array = (double*)calloc(test_amount, sizeof(double));
    uint32_t rand_num = 1022323;

    for (int j = 0; j < SEARCH_ELEMS_AMOUNT; j++){
        #ifdef RANDOM_OPTIMIZATION

        asm volatile(
            "movl %%eax, %%ebx\n\t"
            "shll $13, %%ebx\n\t"
            "xorl %%ebx, %%eax\n\t"

            "movl %%eax, %%ebx\n\t"
            "shrl $17, %%ebx\n\t"
            "xorl %%ebx, %%eax\n\t"

            "movl %%eax, %%ebx\n\t"
            "shll $5, %%ebx\n\t"
            "xorl %%ebx, %%eax\n\t"
            : "=a" (rand_num)
            : "a" (rand_num)
            : "ebx"
        );
        index_array[j] = rand_num % test_data.lines;

        #else

        index_array[j] = rand() % test_data.lines;

        #endif
    }

    for (int i = 0; i < test_amount; i++){
        auto start = std::chrono::steady_clock::now();

        for (int j = 0; j < SEARCH_ELEMS_AMOUNT; j++){
            hashMapSearchElement(&hashMap, test_data.data_array[index_array[j]]);
        }

        auto end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        search_time_array[i] = elapsed.count();
    }

    hashMapDtor(&hashMap);

    double average_time = 0;

    fprintf(file, "Test index,Time ms\n");
    for (uint32_t i = 0; i < test_amount; i++){
        fprintf(file, "%u,%.2f\n", i, search_time_array[i]);
        average_time += search_time_array[i];
    }

    average_time /= test_amount;
    double std_deviation = standartDeviationTime(search_time_array, test_amount, average_time);

    printf("AVERAGE SEARCH TIME: %.2f ms\n", average_time);
    printf("STANDART DEVIATION : %.2f ms\n", std_deviation);
    printf("PERCENTAGE OF ERROR: %.2f\n", std_deviation / average_time * 100);

    free(search_time_array);
    free(index_array);
}

double standartDeviationTime(double* time_array, uint32_t length, double average){
    assert(time_array);

    double standart_deviation = 0;
    for (uint32_t i = 0; i < length; i++){
        standart_deviation += (time_array[i] - average) * (time_array[i] - average);
    }
    standart_deviation = sqrt((float)standart_deviation / length);

    return standart_deviation;
}

float standartDeviationCollisions(HashMap* hashMap, float average){
    assert(hashMap);

    float standart_deviation = 0;
    for (uint32_t i = 0; i < hashMap->capacity; i++){
        uint32_t elems = hashMap->lists[i].elements_amount;
        standart_deviation += (elems - average) * (elems - average);
    }
    standart_deviation = sqrt(standart_deviation / hashMap->capacity);

    return standart_deviation;
}
