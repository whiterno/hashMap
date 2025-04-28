cmake_minimum_required(VERSION 3.11)

project(hashMap)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2 -mavx2 -g")

option(SEARCH_ONLY OFF)
option(STRCMP_OPTIMIZATION OFF)
option(STRCMP_OPTIMIZATION_INLINE OFF)
option(RANDOM_OPTIMIZATION OFF)
option(O3 OFF)
option(CRC32_OPTIMIZATION OFF)
option(REMAINDER_OPTIMIZATION OFF)

if (SEARCH_ONLY)
    add_compile_definitions(SEARCH_ONLY)
endif()

if (STRCMP_OPTIMIZATION)
    add_compile_definitions(STRCMP_OPTIMIZATION)
endif()

if (STRCMP_OPTIMIZATION_INLINE)
    add_compile_definitions(STRCMP_OPTIMIZATION_INLINE)
endif()

if (RANDOM_OPTIMIZATION)
    add_compile_definitions(RANDOM_OPTIMIZATION)
endif()

if (CRC32_OPTIMIZATION)
    add_compile_definitions(CRC32_OPTIMIZATION)
endif()

if (REMAINDER_OPTIMIZATION)
    add_compile_definitions(REMAINDER_OPTIMIZATION)
endif()

if (O3)
    add_compile_options(-O3)
endif()

enable_language(ASM_NASM)

set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/my_strcmp/my_strcmp.s
    PROPERTIES
    LANGUAGE ASM_NASM
    COMPILE_FLAGS "-f elf64"
)

add_executable(main src/main.cpp src/hash_map.cpp src/hash_funcs.cpp src/list.cpp src/utils.cpp src/tests.cpp my_strcmp/my_strcmp.s)
target_include_directories(main PUBLIC include)
