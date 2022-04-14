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
#include <vector>

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

    friend std::ostream &operator<<(std::ostream &os, const SourceLocation &location) {
        os << *location.filename << ":" << location.line << ":" << location.column;
        return os;
    }
};

/// Token
class Token {
    friend class Lexer;

private:
    static const std::unordered_map<int, const char *> TagMap;

    static const std::unordered_map<std::string, int> KwMap;

    static const std::unordered_map<int, int> PrecMap;

public:
    // token enum
    enum {
        // single char punctuate
        NEW_LINE = '\n',// 10
        LP = '(',// 40
        RP = ')',// 41
        Mul = '*',// 42
        Add = '+',// 43
        Comma = ',',// 44
        Sub = '-',// 45
        Div = '/',// 47
        Semi = ';',// 59
        Lt = '<',// 60
        Assign = '=',// 61
        Gt = '>',// 62

        // multi char punctuate
        Dsemi = 130,// ";;"
        Le,// "<="
        Ge,// ">="
        Eq,// "=="
        Ne,// "!="

        // Keywords
        Let = 140,// "let"
        And,//"and"
        In,// "in"
        If,// "if"
        Then,// "then"
        Else,// "else"
        While,// "while"
        Do,// "do"
        Done,// "done"
        Fst,// "fst"
        Snd,// "snd"
        Cons,// "(, )"

        // Keywords_todo
        // Rec,
        // Fun,

        // data type enum
        Var = 180,
        Unit,// "()"
        Bool,
        Int,
        Float,
        String,

        // aux token kind
        INVALID = -3,
        NOTOKEN = -2,
        END = -1,// also START
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

    // Aux: KwMap & TagMap & PrecMap
    static const char *TagLookup(int tag) {
        auto ret = TagMap.find(tag);
        if (ret == TagMap.end()) { return nullptr; }
        return ret->second;
    }

    static int KwLookup(std::string kw) {
        auto ret = KwMap.find(kw);
        if (ret == KwMap.end()) return Token::INVALID;
        return ret->second;
    }

    static bool KwIs(std::string kw) {
        return Token::INVALID != KwLookup(kw);
    }

    static std::vector<const char *> KwList() {
        std::vector<const char *> ret{};
        for (auto item:KwMap) ret.push_back(item.first.c_str());
        return ret;
    }

    static int PrecLookup(int tag) {
        auto ret = PrecMap.find(tag);
        if (ret == PrecMap.end()) { return -1; }
        return ret->second;
    }

    bool IsEOF() const { return tag == END; }

    bool IsConstant() const {
        return tag == Int || tag == Float || tag == String || tag == Bool || tag == Unit;
    }

    bool IsUnary() const { return tag == '+' || tag == '-'; }

    bool IsBinary() const {
        return tag == '+' || tag == '-' || tag == '*' || tag == '/' || tag == '<' || tag == '>' || tag == Le ||
               tag == Eq ||
               tag == Ne || tag == Ge || tag == Cons;
    }

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

    explicit TokenSequence(Token *token) {
        TokenSequence();
        InsertBack(token);
    }

    explicit TokenSequence(TokenList *tokenList)
            : tokenList(tokenList),
              _begin(tokenList->begin()),
              _end(tokenList->end()) {}

    TokenSequence(TokenList *tokenList,
                  TokenList::iterator begin,
                  TokenList::iterator end)
            : tokenList(tokenList), _begin(begin), _end(end) {}

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

    /// Expect
    // Expect the Peek token. Expect its token kind.
    const Token *Expect(int expect);

    /// Expect
    // Expect the Next Peek token.
    const Token *ExpectNext(int expect) {
        Next();
        return Expect(expect);
    }

    /// Try
    // Try the Peek token. If matching its token kind, then Next.
    // Try = Test + Next
    bool Try(int tag) {
        if (Peek()->tag == tag) {
            Next();
            return true;
        }
        return false;
    }

    /// Test
    // Test the Peek token, Check its token kind.
    bool Test(int tag) { return Peek()->tag == tag; }

    /// Next
    // Get the Peek token. Then go next.
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

    /// PutBack
    // Put back the prior Peek token.
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

    /// PeekPrior
    // Get the prior Peek token. First Putback then Next.
    // Design: Illegal and Impossible for the Logic Stack!
    const Token *PeekPrior() {
        PutBack();
        auto ret = Peek();
        Next();
        return ret;
    }

    /// Peek
    // Get the current Peek token.
    const Token *Peek() const;

    /// PeekNext
    // Get the next Peek token. First Next then PutBack.
    const Token *PeekNext() {
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

    friend std::ostream &operator<<(std::ostream &os, TokenSequence &sequence);

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
