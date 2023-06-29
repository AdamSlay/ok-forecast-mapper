cmake_minimum_required(VERSION 3.22)
project(png_mapper_exe)

set(CMAKE_CXX_STANDARD 20)
set(spdlog_DIR /mapper/spdlog/cmake)
list(APPEND CMAKE_PREFIX_PATH
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/pngwriter" ;
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/zlib" ;
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/libpng" ;
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/freetype" ;
        )
include_directories("/mapper/spdlog/include")
find_package(PNGwriter REQUIRED)

add_executable(
        png_mapper_exe
        src/Shape.cpp
        src/Shape.h
        src/CSVReader.cpp
        src/CSVReader.h
        src/utils.cpp
        src/utils.h
        src/draw.cpp
        src/draw.h
        src/main.cpp)

if(PNGwriter_FOUND)
    target_link_libraries(png_mapper_exe PRIVATE PNGwriter::PNGwriter)
endif(PNGwriter_FOUND)

