//
// Created by leo on 2022/4/4.
//

#include "Parser.h"


void Parser::Output(std::ostream &out) const {}

ExpaConstant *Parser::ParseFloat(const Token *token) {
    const std::string &str = token->str;
    float val = 0.0;
    try {
        val = stof(str);
    } catch (const std::out_of_range &oor) {
        CompileError(token, "float out of range");
    }
    return ExpaConstant::New(token, val);
}

ExpaConstant *Parser::ParseInt(const Token *token) {
    const std::string &str = token->str;
    float val = 0.0;
    try {
        val = stoi(str);
    } catch (const std::out_of_range &oor) {
        CompileError(token, "int out of range");
    }
    return ExpaConstant::New(token, val);
}


ExpaConstant *Parser::ParseString(const Token *token) {
    return ExpaConstant::New(token, token->str);
}

ExpaConstant *Parser::ParseConstant(const Token *token) {
    switch (token->tag) {
        case Token::Int:
            return ParseInt(token);
        case Token::Float:
            return ParseFloat(token);
        case Token::String:
            return ParseString(token);
        default:
            CompileError(token, "unexpected contrant");
            return nullptr;
    }
}

Expa *Parser::ParseExpa(Expa *expa) {
    auto token = _ts.Next();
    if (token->IsEOF()) CompileError(token, "premature end of input");
    switch (token->tag) {
        case Token::Var:
//            ParseVar();
            break;
        case Token::Int:
        case Token::Float:
        case Token::String:
        case Token::Bool:
        case Token::Unit:
            ParseConstant(token);
        case Token::If:
            ParseExpaIf();
            break;
        case Token::While:
            ParseExpaWhile();
            break;
        case Token::Let:
            ParseExpaLet();
            break;
        default:
            CompileError(token, "unexpected expa kind");
            break;
    }

}

Var *Parser::ParseVar(const Token *token) {
    return Var::New(token);
}

ExpaIf *Parser::ParseExpaIf() {
    auto tokenIf = Token::New(*_ts.PeekPrior());
    auto token = _ts.Peek();
    auto cond = ParseExp();
    _ts.Expect(Token::Then);  // "then" required
    auto then = ParseExp();
    Exp *els = nullptr;
    if (_ts.Try(Token::Else)) els = ParseExp();  // "else" optional
    return ExpaIf::New(tokenIf, cond, then, els);
}

ExpaWhile *Parser::ParseExpaWhile() {
    auto tokenWhile = Token::New(*_ts.PeekPrior());
    auto token = _ts.Peek();
    auto cond = ParseExp();
    _ts.Expect(Token::Do);  // "do" required
    auto body = ParseExp();
    _ts.Expect(Token::Done);  // "done" required
    return ExpaWhile::New(tokenWhile, cond, body);
}

ExpaLet *Parser::ParseExpaLet() {
    auto tokenLet = Token::New(*_ts.PeekPrior());
    auto token = _ts.Peek();
    VarExpList varExpList;
    do {
        auto var = ParseVar(token);
        _ts.Expect('=');  // "=" required
        auto exp = ParseExp();
        varExpList.push_back(std::pair{var, exp});
    } while (_ts.Try(Token::And));  // "and" optional, repeated

    _ts.Expect(Token::In);  // "in" required
    auto body = ParseExp();
    return ExpaLet::New(tokenLet, body, varExpList);
}





