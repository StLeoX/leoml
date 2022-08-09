#ifndef LEOML_ERROR_H
#define LEOML_ERROR_H

#include "Token.h"
#include <cstdio>
#include <cstdarg>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

inline void CompilePanic(const char *msg)
{
    fprintf(stderr,
            ANSI_COLOR_RED
            "\n===Compile Panic===\n"
            ANSI_COLOR_RESET);
    fprintf(stderr, "%s\n", msg);
    exit(-1);
}

void CompileError(const SourceLocation &loc, const char *format, ...);

void CompileError(const Token *tok, const char *format, ...);

#endif // LEOML_ERROR_H