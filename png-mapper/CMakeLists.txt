cmake_minimum_required(VERSION 3.22)
project(png_mapper_exe)

set(CMAKE_CXX_STANDARD 20)
set(spdlog_DIR /mapper/spdlog/cmake)
list(APPEND CMAKE_PREFIX_PATH
        "/home/png-user/mapper/spack/opt/spack/pngwriter" ;
        "/home/png-user/mapper/spack/opt/spack/zlib" ;
        "/home/png-user/mapper/spack/opt/spack/libpng" ;
        "/home/png-user/mapper/spack/opt/spack/freetype" ;
        )
include_directories("/home/png-user/mapper/spdlog/include")
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

