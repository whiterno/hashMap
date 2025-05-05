#! /bin/bash

compile_options=$1

rm -f build/CMakeCache.txt

cmake -B build ${compile_options}
cmake --build build
