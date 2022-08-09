#ifndef LEOML_ERROR_H
#define LEOML_ERROR_H

#include "syntax/Token.h"
#include <cstdlib>
#include <cstdio>
#include <cstdarg>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Template of CompileError
static void TCompileError(const SourceLocation &loc,
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
    TCompileError(loc, format, args);
    va_end(args);
}

void CompileError(const Token *tok, const char *format, ...) {
    va_list args;
    va_start(args, format);
    TCompileError(tok->loc, format, args);
    va_end(args);
}

#endif // LEOML_ERROR_H