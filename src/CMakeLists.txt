cmake_minimum_required(VERSION 3.8)
project(leoml)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(syntax)

# llvm hdrs
# include_directories(/lib/llvm-11/include)
# project hdrs
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
# project libs
link_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/syntax)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug/syntax) # external build

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
add_executable(leoml main.cpp)
target_link_libraries(leoml
    libleoml_syntax.a)