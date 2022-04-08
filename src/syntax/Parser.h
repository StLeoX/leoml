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
        _program=ParseProgram();
    }

    void Output(std::ostream &out) const;

    Program* ParseProgram();

private:
    /// Parse Decl
    Decl* ParseDecl();

    /// Parse Exp
    Exp *ParseExp();

    /// Parse Expb
    Expb *ParseExpb();

    /// Parse ExpbBinary
    ExpbBinary *ParseExpbBinary(const Token* token);

    /// Parse ExpbUnary
    ExpbUnary *ParseExpbUnary(const Token* token);

    /// Parse ExpbCons
    ExpbCons *ParseExpbCons(const Token* token);

    /// Parse ExpbCompound
    ExpbCompound *ParseExpbCompound(const Token* token);

    /// Parse Expa
    Expa *ParseExpa();

    /// Parse Ident
    Var *ParseVar(const Token *token);

    /// Parse Constant
    ExpaConstant *ParseConstant(const Token *token);

    /// Parse ExpaIf
    ExpaIf *ParseExpaIf(const Token* token);

    /// Parse ExpaWhile
    ExpaWhile *ParseExpaWhile(const Token* token);

    /// Parse ExpaLet
    ExpaLet *ParseExpaLet(const Token* token);
};


#endif //LEOML_PARSER_H
