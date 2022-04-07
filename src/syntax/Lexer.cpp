//
// Created by leo on 2022/3/29.
//

#include "Lexer.h"

Token *Lexer::MakeToken(int tag) {
    _token.tag = tag;
    auto &str = _token.str;
    str.resize(0);
    const char *p = _token.loc.lineBegin + _token.loc.column - 1;
    for (; p < _p; ++p) {
        if (p[0] == '\n' && p[-1] == '\\') { str.pop_back(); } else { str.push_back(p[0]); }
    }
    return Token::New(_token);
}

Token *Lexer::MakeNewLine() {
    _token.tag = '\n';
    _token.str = std::string(_p, _p + 1);
    return Token::New(_token);
}

Token *Lexer::Scan() {
    SkipWhiteSpace();
    Mark();
    // scan newline
    if (Test('\n')) {
        auto ret = MakeNewLine();
        Next();
        return ret;
    }
    auto c = Next();

    // scan token
    switch (c) {
        case ')':
        case '+':
        case '-':
        case '*':
        case '/':
        case ',':
            return MakeToken(c);
        case '(':
            if (Try(')')) return MakeToken(Token::Unit);
            if (Test('*')) {
                SkipComment();
                return Scan();
            }
            return MakeToken('(');
        case ';':
            if (Try(';')) return MakeToken(Token::Semi);
            else return MakeToken(Token::Dsemi);
        case '<':
            if (Try('=')) return MakeToken(Token::Le);
            else return MakeToken(Token::Lt);
        case '>':
            if (Try('=')) return MakeToken(Token::Ge);
            else return MakeToken(Token::Gt);
        case '=':
            if (Try('=')) return MakeToken(Token::Eq);
            else return MakeToken(Token::LetAssign);
        case '!':
            if (Try('=')) return MakeToken(Token::Ne);
            else {
                CompilePanic("Not Support Deref Currently.");
                return nullptr;
            }
        case '0'...'9':
            return SkipNumber();
        case '\"':
            return SkipString();
        case 'a'...'z':
        case 'A'...'Z':
        case '_':
            return SkipIdent();
        case '\0':
            return MakeToken(Token::END);
        default:
            return MakeToken(Token::INVALID);
    }
}

Token *Lexer::ScanKw() {
    std::string word;
    while (!Empty())word.push_back(Next());
    if (Token::KwIs(word)) {
        return MakeToken(Token::KwLookup(word));
    }
    return nullptr;// not match keyword
}

Token *Lexer::ScanIdent() {
    std::string word;
    while (!Empty()) word.push_back(Next());
    return MakeToken(Token::Var);
}

Token *Lexer::ScanString(std::string &word) {
    Next();
    word.resize(0);
    while (!Test('\"')) {
        word.push_back(Next());
    }
    return MakeToken(Token::String);
}

Token *Lexer::SkipIdent() {
    PutBack();
    auto c = Next();
    while (isalpha(c) || c == '_') {
        c = Next();
    }
    PutBack();
    Token *ret = MakeToken(Token::Var);
    // kw IS-A ident
    if (Token::KwIs(ret->str)) ret->tag = Token::KwLookup(ret->str);
    return ret;
}

Token *Lexer::SkipNumber() {
    PutBack();
    int tag = Token::Int;
    auto c = Next();
    while (c == '.' || c == '_' || isdigit(c)) {
        if (c == '.') {
            tag = Token::Float;
        }
        c = Next();
    }
    PutBack();
    return MakeToken(tag);

}

Token *Lexer::SkipString() {
    auto c = Next();
    int len_limit = 128;
    while (c != '\"' && len_limit) {
        c = Next();
        len_limit--;
    }
    if (len_limit <= 0) {
        CompileError(_loc, "unterminated string literal, more than 128 char");
    }
    return MakeToken(Token::String);
}

void Lexer::SkipWhiteSpace() {
    while (isspace(Peek()) && Peek() != '\n') {
        Next();
    }
}

void Lexer::SkipComment() {
    if (Try('(')) {
        while (!Empty()) {
            if (Peek() == '\n')return;
            Next();
        }
        return;
    } else if (Try('*')) {
        while (!Empty()) {
            auto c = Next();
            if (c == '*' && Peek() == ')') {
                Next();
                return;
            }
        }
        CompileError(_loc, "unterminated block comment");
    }
    CompilePanic("comment");
}

void Lexer::Tokenize(TokenSequence &ts) {
    while (true) {
        auto token = Scan();
        if (token->tag == Token::END) {
            if (ts.Empty() || (ts.Back()->tag != Token::NEW_LINE)) {
                auto tmptoken = Token::New(*token);
                tmptoken->tag = Token::NEW_LINE;
                tmptoken->str = "\n";
                ts.InsertBack(tmptoken);
            }// Terminate condition when parsing.
            break;
        } else {
            ts.InsertBack(token);
        }
    }
}

/// Aux
int Lexer::Next() {
    int c = Peek();
    _p++;
    if (c == '\n') {
        _loc.line++;
        _loc.column = 1;
        _loc.lineBegin = _p;
    } else {
        _loc.column++;
    }
    return c;
}

int Lexer::Peek() {
    int c = (uint8_t) (*_p);
    if (c == '\\' && _p[1] == '\n') {
        _p += 2;
        _loc.line++;
        _loc.column = 1;
        _loc.lineBegin = _p;
        return Peek();
    }
    return c;
}

void Lexer::PutBack() {
    int c = *--_p;
    if (c == '\n' && _p[-1] == '\\') {
        _loc.line--;
        _p--;
    } else if (c == '\n') {
        _loc.line--;
    } else {
        _loc.column--;
    }
}