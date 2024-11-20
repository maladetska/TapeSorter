#!/bin/bash

mkdir build
cd build
cmake ../
make
./tests/tape_sorter_tests
