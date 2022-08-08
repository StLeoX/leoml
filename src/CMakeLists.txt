cmake_minimum_required(VERSION 3.16)
project(leoml)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(syntax)

# llvm hdrs
# include_directories(/lib/llvm-11/include)
# project hdrs
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

set(LLVM_LINK_COMPONENTS
    Analysis
    Core
    ExecutionEngine
    InstCombine
    Object
    OrcJIT
    RuntimeDyld
    ScalarOpts
    Support
    native
    )

# leoml
add_executable(leoml main.cpp ${SYNTAX})