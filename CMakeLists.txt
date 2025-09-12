cmake_minimum_required(VERSION 3.26)

project(base LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

include(cmake/compile_options.cmake)

find_package(fmt CONFIG REQUIRED)
find_package(quill CONFIG REQUIRED)
find_package(reflectcpp CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(cpptrace CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)

idk_add_dependency_subdirectory(dpdk)

add_subdirectory(src)
add_subdirectory(tool)
add_subdirectory(test)
