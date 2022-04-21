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
    Program *_program; // the Root of AST
    Scope *currentScope; // current scope

    Parser(const TokenSequence &ts) : _ts(ts), _program(Program::New()) { currentScope = _program->scope; }

    ~Parser() { delete _program; };

public:
    static Parser *New(const TokenSequence &ts) { return new Parser(ts); }

    // main API
    void Parse() {
        _program = ParseProgram();
    }

    void Serialize(std::ostream &os);

    Program *ParseProgram();

private:
    /// Parse Stmt
    Stmt *ParseStmt();

    /// Parse Assign Stmt
    Stmt *ParseAssignStmt(const Token *token);

    /// Parse FuncStmt
    Func *ParseFunc(const Token *token);

    /// Parse FuncCall
    FuncCall *ParseFuncCall(const Token *token);

    /// Parse Exp
    Exp *ParseExp();

    /// Parse Expb
    Expb *ParseExpb();

    /// Parse ExpbBinary
    ExpbBinary *ParseExpbBinary(const Token *token);

    /// Parse ExpbBinary. Precedence Aux
    Expb *ParseExpbBinaryRHS(int prec, Expb *lhs);

    /// Parse ExpbUnary
    ExpbUnary *ParseExpbUnary(const Token *token);

    /// Parse ExpbCons
    ExpbCons *ParseExpbCons(const Token *token);

    /// Parse ExpbFst
    ExpbFst *ParseExpbFst(const Token *token);

    /// Parse ExpbSnd
    ExpbSnd *ParseExpbSnd(const Token *token);

    /// Parse ExpbCompound
    ExpbCompound *ParseExpbCompound(const Token *token);

    /// Parse Expa
    Expb *ParseExpa();

    /// Parse Ident
    Var *ParseVar(const Token *token);

    /// Parse Constant
    ExpaConstant *ParseConstant(const Token *token);

    /// Parse ExpaIf
    ExpaIf *ParseExpaIf(const Token *token);

    /// Parse ExpaWhile
    ExpaWhile *ParseExpaWhile(const Token *token);

    /// Parse ExpaLet
    ExpaLet *ParseExpaLet(const Token *token);

    /// Parse ExpaParen
    Expb *ParseExpaParen(const Token *token);
};


#endif //LEOML_PARSER_H
