#!/bin/bash

rmdir build
mkdir build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -B build
cd build 
make -r all
.\openGlTwo.exe
cd ..