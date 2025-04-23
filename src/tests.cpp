#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <immintrin.h>
#include <chrono>

#include "hash_funcs.h"
#include "hash_map.h"
#include "list.h"
#include "utils.h"
#include "tests.h"

void testAll(const char* filename, uint32_t test_amount){
    size_t file_size = 0;
    char* text = readFile("book.txt", &file_size);

    uint32_t lines = countLines(text, file_size);

    TestData test_data = {.data_array = buildStringArray(text, lines),
                          .lines = lines};

    printf("====================\n");
    TEST(crc32);
    printf("====================\n");
    TEST(murmur3);
    printf("====================\n");

    free(text);
    free(test_data.data_array);
}

void test(hash_t hash_func, Filenames filenames, TestData test_data, uint32_t test_amount){
    FILE* collison_file = fopen(filenames.collision_filename, "w");
    testCollisions(collison_file, hash_func, test_data);
    fclose(collison_file);

    FILE* build_file = fopen(filenames.build_filename, "w");
    testBuildTime(build_file, hash_func, test_data, test_amount);
    fclose(build_file);
}

void testCollisions(FILE* file, hash_t hash_func, TestData test_data){
    assert(file);

    HashMap hashMap = hashMapCtor(hash_func, BASE_HASH_MAP_CAPACITY);

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

    printf("AVERAGE COLLISIONS: %f\n", average_collision);
    printf("STANDART DEVIATION: %f\n", standartDeviationCollisions(&hashMap, average_collision));

    hashMapDtor(&hashMap);
}

void testBuildTime(FILE* file, hash_t hash_func, TestData test_data, uint32_t test_amount){
    assert(file);

    int64_t* build_time_array = (int64_t*)calloc(test_amount, sizeof(int64_t));

    for (uint32_t i = 0; i < test_amount; i++){
        HashMap hashMap = hashMapCtor(hash_func, BASE_HASH_MAP_CAPACITY);

        int64_t start_time = __rdtsc();

        for (int j = 0; j < test_data.lines; j++){
            hashMapAddElement(&hashMap, test_data.data_array[j]);
        }

        int64_t end_time = __rdtsc();

        hashMapDtor(&hashMap);

        build_time_array[i] = end_time - start_time;
    }

    uint64_t average_time = 0;

    fprintf(file, "Test index,Time\n");
    for (uint32_t i = 0; i < test_amount; i++){
        fprintf(file, "%u,%ld\n", i, build_time_array[i]);
        average_time += build_time_array[i];
    }

    average_time /= test_amount;

    printf("AVERAGE BUILD TIME: %ld\n", average_time);
    printf("STANDART DEVIATION: %ld\n", standartDeviationTime(build_time_array, test_amount, average_time));

    free(build_time_array);
}

int64_t standartDeviationTime(int64_t* time_array, uint32_t length, uint32_t average){
    assert(time_array);

    int64_t standart_deviation = 0;
    for (uint32_t i = 0; i < length; i++){
        standart_deviation += (time_array[i] - average) * (time_array[i] - average);
    }
    standart_deviation = (int64_t)sqrt((float)standart_deviation);

    return standart_deviation;
}

float standartDeviationCollisions(HashMap* hashMap, float average){
    assert(hashMap);

    float standart_deviation = 0;
    for (uint32_t i = 0; i < hashMap->capacity; i++){
        uint32_t elems = hashMap->lists[i].elements_amount;
        standart_deviation += (elems - average) * (elems - average);
    }
    standart_deviation = sqrt(standart_deviation);

    return standart_deviation;
}
