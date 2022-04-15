//
// Created by leo on 2022/4/4.
//


#include "Token.h"
#include "error.hpp"

const std::unordered_map<int, const char *> Token::TagMap{
        {'(',           ")"},
        {')',           ")"},
        {'+',           "+"},
        {'-',           "-"},
        {'*',           "*"},
        {'/',           "/"},
        {',',           ","},
        {';',           ";"},
        {'<',           "<"},
        {'>',           ">"},
        {'=',           "="},

        {Token::Dsemi,  ";;"},
        {Token::Le,     "<="},
        {Token::Ge,     ">="},
        {Token::Eq,     "=="},
        {Token::Ne,     "<>"},
        {Token::Unit,   "()"},
        {Token::An,     "&&"},
        {Token::Or,     "||"},

        // Other Expected Token Kind
        {Token::And,    "and"},
        {Token::In,     "in"},
        {Token::Then,   "then"},
        {Token::Else,   "else"},
        {Token::Do,     "do"},
        {Token::Done,   "done"},
        {Token::Rec,    "rec"},

        // Type Name
        {Token::Unit,   "unit"},
        {Token::Bool,   "bool"},
        {Token::Int,    "int"},
        {Token::Float,  "float"},
        {Token::String, "string"},
};

const std::unordered_map<std::string, int> Token::KwMap{
        {"true",  Token::Bool},
        {"false", Token::Bool},

        {"let",   Token::Let},
        {"rec",   Token::Rec},
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

const std::unordered_map<int, int> Token::PrecMap{
        {Token::Semi,  1},  // lowest

        {Token::If,    2},
        {Token::While, 2},
        {Token::Let,   2},

        {Token::Or,    3},

        {Token::An,    4},

        {Token::Lt,    5},
        {Token::Gt,    5},
        {Token::Le,    5},
        {Token::Ge,    5},
        {Token::Eq,    5},
        {Token::Ne,    5},

        {Token::Add,   6},
        {Token::Sub,   6},

        {Token::Mul,   7},
        {Token::Div,   7},

        {Token::LP,    8},
};

std::ostream &operator<<(std::ostream &os, const Token &token) {
    os << "tag: " << token.tag <<
       "\tstr: " << token.str <<
       "\tloc: " << token.loc;
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


