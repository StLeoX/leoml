include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)

set(SYNTAX Token.cpp Lexer.cpp Parser.cpp ParseTree.cpp Scope.cpp Type.cpp Error.cpp)

add_library(leoml_syntax
    ${SYNTAX})