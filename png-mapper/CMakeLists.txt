cmake_minimum_required(VERSION 3.22)
project(png_mapper_exe)

set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_PREFIX_PATH
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/pngwriter-0.7.0-at5v5itwvex4lepf2vtl4u7a6se2ir6k" ;
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/zlib-1.2.13-lqre6rayoxgowclc232om7fpbl2sbdzv" ;
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/libpng-1.6.37-hhxamvxq3mqh3pt4sv6waqpdkel3ltlc" ;
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/freetype-2.11.1-tdqncbfxdqgdov7lqfok67kpiu5rtfer"
        )

find_package(PNGwriter REQUIRED)

add_executable(png_mapper_exe src/main.cpp src/Shape.cpp src/Shape.h src/CSVReader.cpp src/CSVReader.h)

if(PNGwriter_FOUND)
    target_link_libraries(png_mapper_exe PRIVATE PNGwriter::PNGwriter)
endif(PNGwriter_FOUND)
