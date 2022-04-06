//
// Created by leo on 2022/4/4.
//

#ifndef LEOML_PARSER_H
#define LEOML_PARSER_H

#include <ostream>
#include "error.hpp"
#include "Lexer.h"
#include "ParseTree.h"

class Parser {
private:
    TokenSequence _ts; // token stream
    Program *_program = nullptr; // The Root of AST
    Token *_errorToken;

    Parser(const TokenSequence &ts) : _ts(ts), _program(Program::New()) {}

    virtual ~Parser() {};

public:
    static Parser *New(const TokenSequence &ts) { return new Parser(ts); }

    // main API
    void Parse() {
        ParseProgram(_program);
    }

    void ParseProgram(Program *program);

    void Output(std::ostream &out) const;

private:
    void ParseDecl(Decl *decl);

    void ParseExp(Exp *exp);

    /// Expb
    Expb *ParseExpb(Expb *expb);

    ExpbBinary *ParseExpbBinary(ExpbBinary *expbBinary);

    ExpbUnary *ParseExpbUnary(ExpbUnary *expbUnary);

    ExpaConstant *ParseExpbCompound(ExpbCompound *expbCompound);

    /// Expa
    /*
     * expa ::= var
     *        | intl
     *        | floatl
     *        | booll
     *        | stringl
     *        | NaN
     *        | if exp then exp else exp
     *        | let var = exp
     *        | while exp do exp end
     * */
    Expa *ParseExpa(Expa *expa);

    Expa *ParseVar(Var *var);

    /// Parse Constant
    /*
     * Constant ::= intl | floatl | booll | stringl | NaN
     * */
    ExpaConstant *ParseConstant(const Token *token);

    ExpaConstant *ParseInt(const Token *token);

    ExpaConstant *ParseFloat(const Token *token);

    ExpaConstant *ParseString(const Token *token);

    ExpaIf *ParseExpaIf(Expa *expaIf);

    ExpaWhile ParseExpaWhile(Expa *expaWhile);


};


#endif //LEOML_PARSER_H
