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
            return nullptr
    }
}

Expa *Parser::ParseExpa(Expa *expa) {
    auto token = _ts.Next();

}

Expa *Parser::ParseVar(Var *var) {

}





