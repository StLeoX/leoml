#ifndef LEOML_ERROR_H
#define LEOML_ERROR_H

#include "Token.h"
#include <cstdio>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"


inline void CompilePanic(const char *msg) {
    fprintf(stderr,
            ANSI_COLOR_RED
            "\n===Compile Panic===\n"
            ANSI_COLOR_RESET);
    fprintf(stderr, "%s\n", msg);
    std::abort();
}

void CompileError(const char *format, ...) {
    fprintf(stderr,
            ANSI_COLOR_RED
            "Compile Error: "
            ANSI_COLOR_RESET);

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");

    exit(-1);
}

// Template of CompileError
static void VError(const SourceLocation &loc,
                   const char *format,
                   va_list args) {
    assert(loc.filename);
    fprintf(stderr,
            "%s:%d:%d: "
            ANSI_COLOR_RED
            "error: "
            ANSI_COLOR_RESET,
            loc.filename->c_str(),
            loc.line,
            loc.column);
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n    ");

    bool sawNoSpace = false;
    int nspaces = 0;
    for (auto p = loc.lineBegin; *p != '\n' && *p != 0; p++) {
        if (!sawNoSpace && (*p == ' ' || *p == '\t')) {
            ++nspaces;
        } else {
            sawNoSpace = true;
            fputc(*p, stderr);
        }
    }

    fprintf(stderr, "\n    ");
    for (unsigned i = 1; i + nspaces < loc.column; ++i)
        fputc(' ', stderr);
    fprintf(stderr, ANSI_COLOR_GREEN
                    "^\n"
                    ANSI_COLOR_RESET);
    std::abort();
}


void CompileError(const SourceLocation &loc, const char *format, ...) {
    va_list args;
    va_start(args, format);
    VError(loc, format, args);
    va_end(args);
}


void CompileError(const Token *tok, const char *format, ...) {
    va_list args;
    va_start(args, format);
    VError(tok->loc, format, args);
    va_end(args);
}


#endif // LEOML_ERROR_H