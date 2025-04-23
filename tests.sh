#! /bin/bash

# mkdir -p plot/crc32
mkdir -p plot/_mm_crc32
# mkdir -p plot/murmur3
# mkdir -p plot/sum
# mkdir -p plot/adler32
# mkdir -p plot/elf

cmake --build build
nice -n -20 ./build/main 1000

# cd plot

# python build_plot.py crc32
# python build_plot.py _mm_crc32
# python build_plot.py murmur3
# python build_plot.py sum
# python build_plot.py adler32
# python build_plot.py elf
