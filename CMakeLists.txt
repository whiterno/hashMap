cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -msse4.2")

project(hashMap)

add_compile_options(-O3 -g)

add_executable(main src/main.cpp src/hash_map.cpp src/hash_funcs.cpp src/list.cpp src/utils.cpp src/tests.cpp)
target_include_directories(main PUBLIC include)
