cmake_minimum_required(VERSION 3.19)
project(leoml)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(syntax)

add_executable(leoml main.cpp)