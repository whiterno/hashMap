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
option(RESIZABLE_USER OFF)
option(LOAD_FACTOR 0)

enable_language(ASM_NASM)

set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/my_strcmp/my_strcmp.s
    PROPERTIES
    LANGUAGE ASM_NASM
    COMPILE_FLAGS "-f elf64"
)

add_executable(main src/main.cpp src/hash_map.cpp src/hash_funcs.cpp src/list.cpp src/utils.cpp src/tests.cpp my_strcmp/my_strcmp.s)
target_include_directories(main PUBLIC include)

if (SEARCH_ONLY)
    target_compile_definitions(main PUBLIC SEARCH_ONLY)
endif()

if (STRCMP_OPTIMIZATION)
    target_compile_definitions(main PUBLIC STRCMP_OPTIMIZATION)
endif()

if (STRCMP_OPTIMIZATION_INLINE)
    target_compile_definitions(main PUBLIC STRCMP_OPTIMIZATION_INLINE)
endif()

if (RANDOM_OPTIMIZATION)
    target_compile_definitions(main PUBLIC RANDOM_OPTIMIZATION)
endif()

if (CRC32_OPTIMIZATION)
    target_compile_definitions(main PUBLIC CRC32_OPTIMIZATION)
endif()

if (REMAINDER_OPTIMIZATION)
    target_compile_definitions(main PUBLIC REMAINDER_OPTIMIZATION)
endif()

if (O3)
    target_compile_options(main PUBLIC -O3)
endif()
