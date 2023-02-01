#!/bin/bash
mkdir spdlog-lib
git clone https://github.com/gabime/spdlog.git
cd spdlog && mkdir build && cd build || exit
cmake .. && make -j
cd /mapper/src/ || exit
mkdir build images
cd build/ || exit
cmake ../../ && make
