cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2 -mavx2")

project(hashMap)

add_compile_options(-O3 -g -no-pie)

enable_language(ASM_NASM)

set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/my_strcmp/my_strcmp.s
    PROPERTIES
    LANGUAGE ASM_NASM
    COMPILE_FLAGS "-f elf64"
)

add_executable(main src/main.cpp src/hash_map.cpp src/hash_funcs.cpp src/list.cpp src/utils.cpp src/tests.cpp)
target_include_directories(main PUBLIC include)
target_link_libraries(main PUBLIC "${CMAKE_SOURCE_DIR}/my_strcmp/my_strcmp.o")
