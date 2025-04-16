cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -msse4.2")

project(hashMap)

add_executable(main src/main.cpp src/hash_map.cpp src/crc32_hash.cpp src/list.cpp)
target_include_directories(main PUBLIC include)
