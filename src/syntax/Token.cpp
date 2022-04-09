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
        {',',          ","},
        {';',          ";"},
        {'<',          "<"},
        {'>',          ">"},
        {'=',          "="},

        {Token::Dsemi, ";;"},
        {Token::Le,    "<="},
        {Token::Ge,    ">="},
        {Token::Eq,    "=="},
        {Token::Ne,    "!="},
        {Token::Unit,  "()"},

        // Other Expected Token Kind
        {Token::And,   "and"},
        {Token::In,    "in"},
        {Token::Then,  "then"},
        {Token::Else,  "else"},
        {Token::Do,    "do"},
        {Token::Done,  "done"},

};

const std::unordered_map<std::string, int> Token::KwMap{
        {"true",  Token::True},
        {"false", Token::False},

        {"let",   Token::Let},
        {"and",   Token::And},
        {"in",    Token::In},

        {"if",    Token::If},
        {"then",  Token::Then},
        {"else",  Token::Else},

        {"while", Token::While},
        {"do",    Token::Do},
        {"done",  Token::Done},

        {"fst",   Token::Fst},
        {"snd",   Token::Snd},
};

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << "tag: " << token.tag <<
       "\tstr: " << token.str <<
       "\tloc: " << token.loc;
    return os;
}

const char *Token::TagLookup(int tag) {
    auto ret = TagMap.find(tag);
    if (ret == TagMap.end()) { return nullptr; }
    return ret->second;
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
        *_begin = filename;
    }
    return *_begin;
}

const Token *TokenSequence::Expect(int expect) {
    auto token = Peek();
    if (!Try(expect)) {
        CompileError(token, "Token `%s` expected, but got `%s`",
                     Token::TagLookup(expect), token->str.c_str());
    }
    return token;
}

std::ostream &operator<<(std::ostream &os, TokenSequence &sequence) {
    while (!sequence.Empty()) {
        os << *(sequence.Next()) << std::endl;
    };
    return os;
}


