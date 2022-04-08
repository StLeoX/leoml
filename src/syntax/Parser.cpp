//
// Created by leo on 2022/4/4.
//

#include "Parser.h"


/// Parse Float
// ParseConstant aux
ExpaConstant *ParseFloat(const Token *token) {
    const std::string &str = token->str;
    float val = 0.0;
    try {
        val = stof(str);
    } catch (const std::out_of_range &oor) {
        CompileError(token, "float out of range");
    }
    return ExpaConstant::New(token, val);
}

/// Parse Int
// ParseConstant aux
ExpaConstant *ParseInt(const Token *token) {
    const std::string &str = token->str;
    float val = 0.0;
    try {
        val = stoi(str);
    } catch (const std::out_of_range &oor) {
        CompileError(token, "int out of range");
    }
    return ExpaConstant::New(token, val);
}

ExpaConstant *Parser::ParseConstant(const Token *token) {
    switch (token->tag) {
        case Token::Int:
            return ParseInt(token);
        case Token::Float:
            return ParseFloat(token);
        case Token::String:
            return ExpaConstant::New(token, token->str);
        case Token::Bool:
            return ExpaConstant::New(token, token->str == "true" ? true : false);
        case Token::Unit:
            return ExpaConstant::New(token);
        default:
            CompileError(token, "unexpected contrant");
            return nullptr;
    }
}

Var *Parser::ParseVar(const Token *token) {
    return Var::New(token);
}

ExpaIf *Parser::ParseExpaIf(const Token *token) {
    auto cond = ParseExp();
    _ts.Expect(Token::Then);  // "then" required
    auto then = ParseExp();
    Exp *els = nullptr;
    if (_ts.Try(Token::Else)) els = ParseExp();  // "else" optional
    return ExpaIf::New(token, cond, then, els);
}

ExpaWhile *Parser::ParseExpaWhile(const Token *token) {
    auto cond = ParseExp();
    _ts.Expect(Token::Do);  // "do" required
    auto body = ParseExp();
    _ts.Expect(Token::Done);  // "done" required
    return ExpaWhile::New(token, cond, body);
}

ExpaLet *Parser::ParseExpaLet(const Token *token) {
    VarExpList varExpList;
    do {
        auto var = ParseVar(token);
        _ts.Expect('=');  // "=" required
        auto exp = ParseExp();
        varExpList.push_back(std::pair{var, exp});
    } while (_ts.Try(Token::And));  // "and" optional, repeated

    _ts.Expect(Token::In);  // "in" required
    auto body = ParseExp();
    return ExpaLet::New(token, body, varExpList);
}

Expa *Parser::ParsePExp(const Token *token) {
    auto exp = Parser::ParseExp();
    _ts.Expect(Token::RP);
    return Expa::New(exp);
}

Expa *Parser::ParseExpa() {
    auto peek = _ts.Next();
    if (peek->IsEOF()) CompileError(peek, "premature end of input");
    switch (peek->tag) {
        case Token::Var:
            return Expa::New(ParseVar(peek));
        case Token::Int:
        case Token::Float:
        case Token::String:
        case Token::Bool:
        case Token::Unit:
            return ParseConstant(peek);
        case Token::If:
            return ParseExpaIf(peek);
        case Token::While:
            return ParseExpaWhile(peek);
        case Token::Let:
            return ParseExpaLet(peek);
        case Token::LP:
            return ParsePExp(peek);
        default:
            CompileError(peek, "unexpected expa");
            return nullptr;
    }
}

ExpbBinary *Parser::ParseExpbBinary(const Token *token) {
    auto lhs = ParseExpb();  // pop lhs
    auto oper = _ts.Next();  // pop operator
    if (oper->IsEOF()) CompileError(oper, "premature end of input");
    if (!oper->IsBinary()) CompileError(oper, "unexpected binary operator");
    auto rhs = ParseExpb();
    return ExpbBinary::New(token, oper->tag, lhs, rhs);
}

ExpbUnary *Parser::ParseExpbUnary(const Token *token) {
    auto oper = _ts.Next();  // pop operator
    if (oper->IsEOF()) CompileError(oper, "premature end of input");
    auto oprand = ParseExpb();  // pop oprand
    return ExpbUnary::New(token, oper->tag, oprand);
}

ExpbCons *Parser::ParseExpbCons(const Token *token) {
    _ts.Expect('(');
    auto first = ParseExpb();
    _ts.Expect(',');
    auto second = ParseExpb();
    _ts.Expect(')');
    return ExpbCons::New(token, first, second);
}

ExpbCompound *Parser::ParseExpbCompound(const Token *token) {
    auto first = ParseExpb();  // pop first
    _ts.Expect(';');
    auto second = ParseExpb();  // pop second
    return ExpbCompound::New(token, first, second);
}

Expb *Parser::ParseExpb() {
    auto peek = _ts.Next();
    auto peek2 = _ts.PeekNext();
    if (peek->IsEOF()) CompileError(peek, "premature end of input");
    // switch
    // First(expa)
    if (peek->tag == Token::Var || peek->IsConstant() || peek->tag == Token::If || peek->tag == Token::While ||
        peek->tag == Token::Let) {
        ParseExpa();
    }
        // First(expbUnary)
    else if (peek->IsUnary()) { ParseExpbUnary(peek); }
        // First(expbCons)
    else if (peek->tag == Token::LP) { ParseExpbCons(peek); }
        // First(expbBinary) \Union First(expbCompound)
        // LeftRecur, but we can use the peek2 :)
    else if (peek2->IsBinary()) {
        ParseExpbBinary(peek);
    } else if (peek2->tag == Token::Semi) {
        ParseExpbCompound(peek);
    }
    return nullptr;
}

Exp *Parser::ParseExp() {
    auto peek = _ts.Next();
    if (peek->IsEOF()) CompileError(peek, "premature end of input");
    auto ret = Exp::New(peek);
    if (peek->tag == Token::Var) { ret->var = Var::New(peek); }
    ret->expbList.push_back(ParseExpb());
    while (auto expb = ParseExpb()) {
        ret->expbList.push_back(expb);
    }
    return ret;
}

Decl *Parser::ParseDecl() {
    auto peek = _ts.Next();
    auto ret = Decl::New();
    // let...
    if (peek->tag == Token::Var) {
        ret->varList.push_back(Var::New(peek));
        if (_ts.Test('(')) {
            ret->varList.push_back((Var::New(_ts.Next())));
            do {
                ret->varList.push_back((Var::New(_ts.Next())));
            } while (_ts.Try(Token::Comma));
            _ts.Expect(')');
        }
        _ts.Expect('=');
        ret->exp = ParseExp();
        _ts.Expect(Token::Dsemi);
    }
        // exp kind
    else {
        ret->exp = ParseExp();
        _ts.Expect(Token::Dsemi);
    }
    return ret;
}

Program *Parser::ParseProgram() {
    auto ret = Program::New();
    auto token = _ts.Next();
    while (!token->IsEOF()) {
        ret->declList.push_back(ParseDecl());
    }
    return ret;
}

std::ostream &operator<<(std::ostream &os, const Parser &parser) {
    os << *parser._program;
    return os;
}
