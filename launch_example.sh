#!/bin/bash

mkdir build
cd build
cmake ../
make
./bin/TapeSorter ../example/config3.yaml
