JIT := jit.h jit.cpp
SRC := $(JIT)
INC := /lib/llvm-11/include /lib/llvm-11/build/include
CC := clang++ -Og -w -g
CFLAGS := -std=c++17 -rdynamic
llvm_flags := `llvm-config --cxxflags --ldflags --system-libs --libs core orcjit native executionengine runtimedyld`
leoml : $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(llvm_flags)