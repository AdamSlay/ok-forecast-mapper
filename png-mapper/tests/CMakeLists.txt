cmake_minimum_required(VERSION 3.22)
project(my_project)

set(CMAKE_CXX_STANDARD 20)

set(spdlog_DIR /mapper/spdlog/cmake)
list(APPEND CMAKE_PREFIX_PATH
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/pngwriter" ;
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/zlib" ;
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/libpng" ;
        "/spack/opt/spack/linux-ubuntu22.04-aarch64/gcc-11.3.0/freetype" ;
        )
include_directories("/mapper/spdlog/include")
find_package(PNG REQUIRED)
find_package(PNGwriter REQUIRED)

include(FetchContent)
FetchContent_Declare(
        googletest
        DOWNLOAD_EXTRACT_TIMESTAMP True
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        unit_tests
        main_test.cc
        ../src/utils.h
        ../src/utils.cpp
)

target_link_libraries(
        unit_tests
        GTest::gtest_main
        PNGwriter::PNGwriter
)
include(GoogleTest)
gtest_discover_tests(unit_tests)
