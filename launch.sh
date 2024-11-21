#!/bin/bash

mkdir build
cd build
cmake ../
make
cd ./tests
./tape_sorter_tests
