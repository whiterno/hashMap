#! /bin/bash

mkdir -p plot/crc32
mkdir -p plot/murmur3

cmake --build build
nice -n -20 ./build/main

cd plot

python build_plot.py crc32
python build_plot.py murmur3
