//
// Created by leo on 2022/3/29.
//

#ifndef LEOML_LEXER_H
#define LEOML_LEXER_H

#include <cassert>
#include <cstring>
#include "Token.h"
#include "error.hpp"

/// Trie
// To match keywords.
struct Trie {
    int nex[100000][26], cnt;
    bool exist[100000];

    Trie() = default;

    void insert(const char *s, int l) {
        int p = 0;
        for (int i = 0; i < l; i++) {
            int c = s[i] - 'a';
            if (!nex[p][c]) nex[p][c] = ++cnt;
            p = nex[p][c];
        }
        exist[p] = 1;
    }

    bool find(const char *s, int l) {
        int p = 0;
        for (int i = 0; i < l; i++) {
            int c = s[i] - 'a';
            if (!nex[p][c]) return 0;
            p = nex[p][c];
        }
        return exist[p];
    }

    bool first(int c) {
        int j = c - 'a';
        if (!nex[0][j]) return 0;
        return exist[nex[0][j]];
    }

};

/// Lexer
class Lexer {
private:
    const std::string *_text;  // source text
    const char *_p;  // text offset
    SourceLocation _loc{}; // location
    Token _token{Token::END}; // current token
    Trie _kwTrie{}; // keywords trie

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
        ret->BuildKwTrie();
        return ret;
    }

    virtual ~Lexer() = default;

    Lexer(const Lexer &other) = delete;

    Lexer &operator=(const Lexer &other) = delete;

    // main API
    void Tokenize(TokenSequence &ts);

private:
    // build kw trie
    void BuildKwTrie();

    Token *MakeToken(int tag);

    Token *MakeNewLine();

    // main Scan
    Token *Scan();

    // scan keyword
    Token *ScanKw();

    // scan ident
    Token *ScanIdent();

    // scan string
    Token *ScanString(std::string& word);

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
