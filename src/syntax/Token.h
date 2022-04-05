//
// Created by leo on 2022/3/29.
//


#ifndef LEOML_TOKEN_H
#define LEOML_TOKEN_H

#include <any>
#include <cstdint>
#include <string>
#include <set>
#include <list>
#include <unordered_map>
#include <cassert>
#include <cstdarg>
#include <fstream>
#include <ostream>

class Parser;

class Lexer;

class Token;

class TokenSequence;

using TokenList = std::list<const Token *>;

struct SourceLocation {
    const std::string *filename;
    const char *lineBegin;
    unsigned line;
    unsigned column;

    const char *Begin() const {
        return lineBegin + column - 1;
    }
};

/// Token
class Token {
    friend class Lexer;

private:
    static const std::unordered_map<int, const char *> TagMap;

    static const std::unordered_map<std::string, int> KwMap;
public:
    // token enum
    enum {
        // single char token
        LP = '(',
        RP = ')',
        Add = '+',
        Sub = '-',
        Mul = '*',
        Div = '/',
        Semi = ';',
        Lt = '<',
        Gt = '>',
        LetAssign = '=',
        NEW_LINE = '\n',

        // multi char token
        Le,
        Ge,
        Eq,
        Ne,
        True,
        False,
        Nan,

        // Keywords
        Let,
        Fun,
        If,
        Then,
        Else,
        While,
        Do,
        End,
        Cons,
        Car,
        Cdr,
        Empty,

        // data type enum
        Var,
        Int,
        Bool,
        Float,
        String,

        // aux token kind
        INVALID,
        END,
        NOTOKEN = -1,
    };

    int tag;
    std::string str;
    SourceLocation loc;

    explicit Token(int tag) : tag(tag) {};

    static Token *New(int tag) { return new Token(tag); };

    static Token *New(const Token &other) { return new Token(other); };

    static Token *New(int tag, const SourceLocation &loc, const std::string &str) { return new Token(tag, loc, str); };

    Token &operator=(const Token &other) {
        tag = other.tag;
        loc = other.loc;
        str = other.str;
        return *this;
    }

    virtual ~Token() = default;

    static const char *Lookup(int tag_) {
        auto ret = TagMap.find(tag_);
        if (ret == TagMap.end()) { return nullptr; }
        return ret->second;
    }

    bool IsEOF() const { return tag == Token::END; }

    friend std::ostream &operator<<(std::ostream &os, const Token &token);

private:

    Token(int tag, const SourceLocation &loc, const std::string &str)
            : tag(tag), loc(loc), str(str) {}

    Token(const Token &other) {
        *this = other;
    }

};

/// TokenSequence (TokenStream)
/// An abstraction of all tokens to be parse.
class TokenSequence {

public:
    TokenSequence() : tokenList(new TokenList()),
                      _begin(tokenList->begin()), _end(tokenList->end()) {}

    explicit TokenSequence(Token *tok) {
        TokenSequence();
        InsertBack(tok);
    }

    explicit TokenSequence(TokenList *tokList)
            : tokenList(tokList),
              _begin(tokList->begin()),
              _end(tokList->end()) {}

    TokenSequence(TokenList *tokList,
                  TokenList::iterator begin,
                  TokenList::iterator end)
            : tokenList(tokList), _begin(begin), _end(end) {}

    ~TokenSequence() {}

    TokenSequence(const TokenSequence &other) { *this = other; }

    const TokenSequence &operator=(const TokenSequence &other) {
        tokenList = other.tokenList;
        _begin = other._begin;
        _end = other._end;
        return *this;
    }

    void Copy(const TokenSequence &other) {
        tokenList = new TokenList(other._begin, other._end);
        _begin = tokenList->begin();
        _end = tokenList->end();
        for (auto iter = _begin; iter != _end; ++iter)
            *iter = Token::New(**iter);
    }

    const Token *Expect(int expect);

    bool Try(int tag) {
        if (Peek()->tag == tag) {
            Next();
            return true;
        }
        return false;
    }

    bool Test(int tag) { return Peek()->tag == tag; }

    const Token *Next() {
        auto ret = Peek();
        if (!ret->IsEOF()) {
            ++_begin;
            Peek(); // May skip newline token
        } else {
            ++exceed_end;
        }
        return ret;
    }

    void PutBack() {
        assert(_begin != tokenList->begin());
        if (exceed_end > 0) {
            --exceed_end;
        } else {
            --_begin;
            if ((*_begin)->tag == Token::NEW_LINE)
                PutBack();
        }
    }

    const Token *Peek() const;

    const Token *Peek2() {
        if (Empty())
            return Peek(); // Return the Token::END
        Next();
        auto ret = Peek();
        PutBack();
        return ret;
    }

    const Token *Back() const {
        auto back = _end;
        return *--back;
    }

    void PopBack() {
        assert(!Empty());
        assert(_end == tokenList->end());
        auto size_eq1 = tokenList->back() == *_begin;
        tokenList->pop_back();
        _end = tokenList->end();
        if (size_eq1)
            _begin = _end;
    }

    TokenList::iterator Mark() { return _begin; }

    void ResetTo(TokenList::iterator mark) { _begin = mark; }

    bool Empty() const { return Peek()->IsEOF(); }

    void InsertBack(TokenSequence &ts) {
        auto pos = tokenList->insert(_end, ts._begin, ts._end);
        if (_begin == _end) {
            _begin = pos;
        }
    }

    void InsertBack(const Token *tok) {
        auto pos = tokenList->insert(_end, tok);
        if (_begin == _end) {
            _begin = pos;
        }
    }

    // If there is preceding newline
    void InsertFront(TokenSequence &ts) {
        auto pos = GetInsertFrontPos();
        _begin = tokenList->insert(pos, ts._begin, ts._end);
    }

    void InsertFront(const Token *tok) {
        auto pos = GetInsertFrontPos();
        _begin = tokenList->insert(pos, tok);
    }

    bool IsBeginOfLine() const;

    TokenSequence GetLine();

    void SetParser(Parser *parser) { _parser = parser; }

    void Output(std::ostream &out) const;

private:
    TokenList::iterator GetInsertFrontPos() {
        auto pos = _begin;
        if (pos == tokenList->begin())
            return pos;
        --pos;
        while (pos != tokenList->begin() && (*pos)->tag == Token::NEW_LINE)
            --pos;
        return ++pos;
    }

    TokenList *tokenList;
    mutable TokenList::iterator _begin;
    TokenList::iterator _end;
    Parser *_parser{nullptr};
    int exceed_end{0};
};

#endif //LEOML_TOKEN_H
