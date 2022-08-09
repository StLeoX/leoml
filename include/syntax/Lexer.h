//
// Created by leo on 2022/3/29.
//


#ifndef LEOML_LEXER_H
#define LEOML_LEXER_H

#include "Token.h"
#include <cassert>
#include <cstring>

/// Lexer
class Lexer {
private:
    const std::string *_text;  // source text
    const char *_p;  // text offset
    SourceLocation _loc{}; // location
    Token _token{Token::END}; // current token

    Lexer(const Token *token) : Lexer(&token->str, token->loc) {}

    Lexer(const std::string *text, const SourceLocation &loc) : Lexer(text, loc.filename, loc.line, loc.column) {}

    Lexer(const std::string *text, const std::string *filename = nullptr, unsigned line = 1,
          unsigned column = 1) {
        _token = Token(Token::END);
        _text = text;
        _p = &(*_text)[0];
        _loc = {filename, _p, line, 1};
    }

public:
    static Lexer *New(const std::string *text, const std::string *filename) {
        auto ret = new Lexer(text, filename);
        return ret;
    }

    virtual ~Lexer() = default;

    Lexer(const Lexer &other) = delete;

    Lexer &operator=(const Lexer &other) = delete;

    // main API
    void Tokenize(TokenSequence &ts);

private:
    Token *MakeToken(int tag);

    Token *MakeNewLine();

    // main Scan
    Token *Scan();

    // scan keyword
    Token *ScanKw();

    // scan ident
    Token *ScanIdent();

    // scan string
    Token *ScanString(std::string &word);

    // var
    // var ::= [a-zA-Z_][a-zA-Z_0-9]*
    Token *SkipIdent();

    // int & float
    Token *SkipNumber();

    // string, support newline
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
