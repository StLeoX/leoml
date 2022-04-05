//
// Created by leo on 2022/4/4.
//

#include "Token.h"
#include "error.hpp"

const std::unordered_map<int, const char *> Token::TagMap{
        {'(',          ")"},
        {')',          "("},
        {'+',          "+"},
        {'-',          "-"},
        {'*',          "*"},
        {'/',          "/"},
        {'<',          "<"},
        {'>',          ">"},
        {';',          ";"},
        {'=',          "="},

        {Token::Le,    "<="},
        {Token::Ge,    ">="},
        {Token::Eq,    "=="},
        {Token::Ne,    "!="},
        {Token::True,  "true"},
        {Token::False, "false"},
        {Token::Nan,   "NaN"},
};

const std::unordered_map<std::string, int> Token::KwMap{
        {"let",   Token::Let},
        {"fun",   Token::Fun},
        {"if",    Token::If},
        {"then",  Token::Then},
        {"else",  Token::Else},
        {"while", Token::While},
        {"do",    Token::Do},
        {"end",   Token::End},
        {"cons",  Token::Cons},
        {"car",   Token::Car},
        {"cdr",   Token::Cdr},
        {"empty", Token::Empty},
};

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << "tag: " << token.tag << " str: " << token.str;
    return os;
}

TokenSequence TokenSequence::GetLine() {
    auto begin = _begin;
    while (_begin != _end && (*_begin)->tag != Token::NEW_LINE)
        ++_begin;
    auto end = _begin;
    return {tokenList, begin, end};
}

bool TokenSequence::IsBeginOfLine() const {
    if (_begin == tokenList->begin())
        return true;

    auto pre = _begin;
    --pre;


    return ((*pre)->tag == Token::NEW_LINE ||
            (*pre)->loc.filename != (*_begin)->loc.filename);
}


const Token *TokenSequence::Peek() const {
    static auto eof = Token::New(Token::END);
    if (_begin != _end && (*_begin)->tag == Token::NEW_LINE) {
        ++_begin;
        return Peek();
    } else if (_begin == _end) {
        if (_end != tokenList->begin())
            *eof = *Back();
        eof->tag = Token::END;
        return eof;
    } else if (_parser && (*_begin)->tag == Token::Var) {
        auto filename = Token::New(*(*_begin));
        filename->tag = Token::String;
        // todo: func call
//        filename->setStr("\"" + parser_->CurFunc()->Name() + "\"");
        *_begin = filename;
    }
    return *_begin;
}


const Token *TokenSequence::Expect(int expect) {
    auto token = Peek();
    if (!Try(expect)) {
        CompileError(token, "'%s' expected, but got '%s'",
                     Token::Lookup(expect), token->str.c_str());
    }
    return token;
}


void TokenSequence::Output(std::ostream& out) const {
    unsigned lastLine = 0;
    auto ts = *this;
    while (!ts.Empty()) {
        auto token = ts.Next();
        if (lastLine != token->loc.line) {
            out << "\n";
            for (unsigned i = 0; i < token->loc.column; ++i)
                out << " ";
        }
        out << token;
        lastLine = token->loc.line;
    }
    out << "\n";
}


