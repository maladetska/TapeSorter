#!/bin/bash

CONFIG_FILE=$1

mkdir build
cd build
cmake ../
make
./bin/TapeSorter $CONFIG_FILE
