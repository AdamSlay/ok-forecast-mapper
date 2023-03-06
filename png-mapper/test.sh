#!/bin/bash
cd /mapper/tests/ || exit
cmake -S . -B build
cmake --build build
cd build || exit
ctest