cmake_minimum_required(VERSION 3.22)
project(png_mapper_exe)

set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_PREFIX_PATH
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/pngwriter-0.7.0" ;
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/zlib-1.2.13" ;
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/libpng-1.6.37" ;
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/freetype-2.11.1"
        )

find_package(PNGwriter REQUIRED)

add_executable(png_mapper_exe src/main.cpp src/Shape.cpp src/Shape.h src/CSVReader.cpp src/CSVReader.h)

if(PNGwriter_FOUND)
    target_link_libraries(png_mapper_exe PRIVATE PNGwriter::PNGwriter)
endif(PNGwriter_FOUND)
