#! /bin/bash

amount=$1
which=$2
if_plot=$3
shift 3
compile_options=$@

if [[ ${which} == "all" ]]; then
    mkdir -p plot/crc32
    mkdir -p plot/_mm_crc32
    mkdir -p plot/_mm_crc32Unroll
    mkdir -p plot/murmur2
    mkdir -p plot/sum
    mkdir -p plot/adler32
    mkdir -p plot/elf
else
    mkdir -p plot/${which}
fi

rm -f build/CMakeCache.txt

cmake -B build ${compile_options}
cmake --build build

nice -n -20 taskset -c 1 ./build/main ${amount} ${which}

cd plot

if [[ ${if_plot} == "plot" ]]; then
    if [[ ${which} == "all" ]]; then
        python build_plot.py crc32
        python build_plot.py _mm_crc32
        python build_plot.py _mm_crc32Unroll
        python build_plot.py murmur2
        python build_plot.py sum
        python build_plot.py adler32
        python build_plot.py elf
    else
        python build_plot.py ${which}
    fi
fi
