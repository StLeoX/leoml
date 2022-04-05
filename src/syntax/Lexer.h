//
// Created by leo on 2022/3/29.
//

#ifndef LEOML_LEXER_H
#define LEOML_LEXER_H

#include <cassert>

#include "Token.h"
#include "error.hpp"

/// Lexer
class Lexer {
private:
    const std::string *_text;
    SourceLocation _loc;
    Token _token = Token(Token::END); // current
    const char *_p;

public:
    static Lexer New(const std::string *text, const std::string *filename) {
        return Lexer(text, filename);
    }

    explicit Lexer(const Token *token) : Lexer(&token->str, token->loc) {}

    Lexer(const std::string *text, const SourceLocation &loc) : Lexer(text, loc.filename, loc.line, loc.column) {}

    explicit Lexer(const std::string *text, const std::string *filename = nullptr, unsigned line = 1,
                   unsigned column = 1) {
        _token = Token(Token::END);
        _text = text;
        _p = &(*_text)[0];
        _loc = {filename, _p, line, 1};
    }

    virtual ~Lexer() = default;

    Lexer(const Lexer &other) = delete;

    Lexer &operator=(const Lexer &other) = delete;

    // main API
    void Tokenize(TokenSequence &ts);

private:
    //
    Token *Scan();

    Token *MakeToken(int tag);

    Token *MakeNewLine();

    // var
    // var ::= [a-zA-Z_][a-zA-Z_0-9]*
    Token *SkipIdent();

    // int & float
    Token *SkipNumber();

    // string
    Token *SkipString();

    void SkipWhiteSpace();

    void SkipComment();

    // Aux
    bool Empty() const { return *_p == 0; }

    int Peek();

    bool Test(int c) { return Peek() == c; };

    int Next();

    void PutBack();

    bool Try(int c) {
        if (Peek() == c) {
            Next();
            return true;
        }
        return false;
    };

    void Mark() { _token.loc = _loc; };
};

#endif //LEOML_LEXER_H
