cmake_minimum_required(VERSION 3.16)
project(leoml)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(syntax)

include_directories(/lib/llvm-11/include /lib/llvm-11/build/include)

#add_compile_options(`llvm-config --cxxflags --ldflags --system-libs --libs core orcjit native executionengine runtimedyld`)

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

add_executable(leoml ${SYNTAX} main.cpp)