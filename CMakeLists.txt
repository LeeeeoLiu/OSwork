cmake_minimum_required(VERSION 3.6)
project(OSlab)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES os5.cpp)
add_executable(OSlab ${SOURCE_FILES})