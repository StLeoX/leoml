//
// Created by leo on 2022/4/4.
//
// The Abstraction of ParseTree
//


#ifndef LEOML_PARSETREE_H
#define LEOML_PARSETREE_H

#include <list>
#include <unordered_map>
#include <iostream>
#include <ostream>
#include "Token.h"

class Visitor;

/// AST Node Interface
/*
 * Interface:
 *     - Accept;
 *     - operator<<;
 * */
class ParseTreeNode {
public:
    ParseTreeNode() {};

    virtual ~ParseTreeNode() {};

//    virtual void Accept(Visitor *v) = 0;  // Terminate the visiting.

    virtual void Serialize(std::ostream &os) = 0;  // Serialize the node.

};

/// Language Hierarchy Model
class Stmt;  // Stmt

using StmtList = std::list<Stmt *>; // StmtList

class Func;  // Func

class FuncCall;  // FuncCall

class Var;

using VarList = std::list<Var *>; // Varlist

class Exp; // Exp

class Expb; //Expb

using ExpbList = std::list<Expb *>; // Explist

class ExpbBinary;

class ExpbUnary;

class ExpbCompound;

class Expa; //Expa

using ExpPairList = std::list<std::pair<Expa *, Exp *>>;

class ExpaIf;

class ExpaWhile;

class ExpaLet;


/// Program
class Program : public ParseTreeNode {
    template<typename T> friend
    class TreeVisitor;

private:
    Program() : stmtList(new StmtList) {};

public:
    StmtList *stmtList;

    virtual ~Program() { delete stmtList; };

    static Program *New() {
        return new Program();
    }

    virtual void Serialize(std::ostream &os);

};

/// Stmt
/*
 * stmt ::= let funcStmt = exp ;;  # varlist optional
 *        | var;;
 * As the top NoTerminal, stmt works like an union.
 * stmt's old-name is decl.
 * */
class Stmt : public ParseTreeNode {
    template<typename T> friend
    class TreeVisitor;

public:
    // stmt enum
    enum {
        VarStmt = 0,
        FuncAssignStmt,
        VarAssignStmt,
    };

    Var *var;
    Func *func;
    Exp *exp;
    int kind;

    virtual ~Stmt();

    static Stmt *New() { return new Stmt(); }

    virtual void Serialize(std::ostream &os);

};

/// Exp
/*
 * exp ::= var expblist
 *       | expb
 *
 * expblist ::= expb
 *            | expblist expb
 *
 * */
class Exp : public ParseTreeNode {
    template<typename T> friend
    class TreeVisitor;

    friend class Expa;

protected:
    Exp() {}

    const Token *_root;

    Exp(const Token *token) : _root(token), expbList(new ExpbList) {};

public:
    Var *var;
    ExpbList *expbList;

    virtual ~Exp() { delete _root, var, expbList; };

    static Exp *New(const Token *token) { return new Exp(token); }

    virtual void Serialize(std::ostream &os);

};

/// Expb
/* expb ::= expa
 *        | expbBinary
 *        | expbUnary
 *        | expbCons
 *        | expbCompound
 * FIRST
 *     First(expa) = \Union First(\expa_i)
 *     First(expbBinary) = { !LeftRecur! }
 *     First(expbUnary) = { Token::UnaryOpSet }
 *     First(expbCons) = { (, }
 *     First(expbCompound) = { !LeftRecur! }
 * */
class Expb : public Exp {
    template<typename T> friend
    class TreeVisitor;

protected:
    Expb(const Token *token) : Exp(token) {};

public:
    virtual ~Expb() {}

    static Expb *New(const Token *token) { return new Expb(token); }

    // Expb should not be directly serilizated.
//    virtual void Serialize(std::ostream &os);

};

/// Expb Binary
/*
 * expb ::= expa BinaryOp expb
 *
 * BinaryOp: +, -, *, /, <, >, <=, >=, ==, !=
 * */
class ExpbBinary : public Expb {
    template<typename T> friend
    class TreeVisitor;

private:
    Expb *_lhs;
    Expb *_rhs;
    int _op;

    ExpbBinary(const Token *token, int op, Expb *lhs, Expb *rhs) : Expb(token), _lhs(lhs), _rhs(rhs), _op(op) {
    }

public:
    ~ExpbBinary();;

    static ExpbBinary *New(const Token *token, Expb *lhs, Expb *rhs) {
        return new ExpbBinary(token, token->tag, lhs, rhs);
    }

    static ExpbBinary *New(const Token *token, int op, Expb *lhs, Expb *rhs) {
        return new ExpbBinary(token, op, lhs, rhs);
    };

    virtual void Serialize(std::ostream &os);

};

/// Expb Unary
/*
 * expb ::= UnaryOp expa
 * UnaryOp:
 *     +, -
 * */
class ExpbUnary : public Expb {
    template<typename T> friend
    class TreeVisitor;

private:
    Expb *_oprand;
    int _op;

    ExpbUnary(const Token *root, int op, Expb *oprand) : Expb(root), _op(op), _oprand(oprand) {}

public:
    ~ExpbUnary() { delete _oprand; };

    static ExpbUnary *New(const Token *token, Expb *oprand) { return new ExpbUnary(token, token->tag, oprand); }

    static ExpbUnary *New(const Token *token, int op, Expb *oprand) { return new ExpbUnary(token, op, oprand); };

    virtual void Serialize(std::ostream &os);

};

/// Expb Cons
/*
 * expb ::= ( expa, expb )
 * */
class ExpbCons : public Expb {
    template<typename T> friend
    class TreeVisitor;

private:
    Expb *_first;
    Expb *_second;

    ExpbCons(const Token *token, Expb *first, Expb *second) : Expb(token), _first(first), _second(second) {}

public:
    ~ExpbCons();

    static ExpbCons *New(const Token *token, Expb *first, Expb *second) { return new ExpbCons(token, first, second); }

    virtual void Serialize(std::ostream &os);

};

/// Expb Compound
/*
 * expb ::= expb; expb
 * */
class ExpbCompound : public Expb {
    template<typename T> friend
    class TreeVisitor;

private:
    Expa *_first;
    Expb *_second;

    ExpbCompound(const Token *root, int op, Expa *lhs, Expb *rhs) : Expb(root), _first(lhs), _second(rhs) {}

public:
    ~ExpbCompound();

    static ExpbCompound *New(const Token *token, Expa *lhs, Expb *rhs) {
        return new ExpbCompound(token, token->tag, lhs, rhs);
    }

    virtual void Serialize(std::ostream &os);

};

/// Expb Fst
/*
 * expb ::= fst ( expa, expb )
 * */
class ExpbFst : public Expb {
    template<typename T> friend
    class TreeVisitor;

private:
    Expb *_first;
    Expb *_second;

    ExpbFst(const Token *token, Expb *first, Expb *second) : Expb(token), _first(first), _second(second) {}

public:
    ~ExpbFst();

    static ExpbFst *New(const Token *token, Expb *first, Expb *second) { return new ExpbFst(token, first, second); }

    virtual void Serialize(std::ostream &os);

};

/// ExpbSnd
/*
 * expb ::= snd ( expa, expb )
 * */
class ExpbSnd : public Expb {
    template<typename T> friend
    class TreeVisitor;

private:
    Expb *_first;
    Expb *_second;

    ExpbSnd(const Token *token, Expb *first, Expb *second) : Expb(token), _first(first), _second(second) {}

public:
    ~ExpbSnd();

    static ExpbSnd *New(const Token *token, Expb *first, Expb *second) { return new ExpbSnd(token, first, second); }

    virtual void Serialize(std::ostream &os);

};

/// Expa
/*
 * expa ::= var
 *        | expaConstant
 *        | expaIf
 *        | expaWhile
 *        | expaLet
 *        | ( exp )
 * */
class Expa : public Expb {
    template<typename T> friend
    class TreeVisitor;

protected:
    Expa(const Token *token) : Expb(token) {}

public:
    ~Expa() {};

    static Expa *New(Exp *exp) { return new Expa(exp->_root); }

    static Expa *New(const Token *token) { return new Expa(token); }

    // Expa should not be directly serilizated.
//    virtual void Serialize(std::ostream &os);

};

/// Expa Var
class Var : public Expa {
    template<typename T> friend
    class TreeVisitor;

private:
    Var(const Token *token) : Expa(token) {}

public:
    ~Var() { delete _root; };

    static Var *New(const Token *token) { return new Var(token); }

    std::string GetStr() { return _root->str; }

    virtual void Serialize(std::ostream &os);

};

/// Func
/*
 * func ::= var [( varlist )]?  # varlist optional
 * func alias funcStmt
 * */
class Func : public Expa {
    template<typename T> friend
    class TreeVisitor;

protected:
    Func(const Token *token) : Expa(token), paramList(new VarList) {}

public:
    std::string name;
    Exp *body;
    VarList *paramList;

    virtual ~Func() { delete body, paramList; }

    static Func *New(const Token *token) { return new Func(token); }

    virtual void Serialize(std::ostream &os);

};

/// FuncCall
/*
 * funcCall extends funcStmt and expa:
 *     funcCall has the return value as Exp*
 * */
class FuncCall : public Func {
    template<typename T> friend
    class TreeVisitor;

private:
    FuncCall(const Token *token) : Func(token), argList(new ExpbList) {}

public:
    Exp *retValue;
    ExpbList *argList;

    ~FuncCall() { delete retValue, argList; }

    static FuncCall *New(const Token *token) { return new FuncCall(token); }

    virtual void Serialize(std::ostream &os);

};

/// Expa Constant
/*
 * Constant ::= intl | floatl | booll | stringl | Unit
 * */
class ExpaConstant : public Expa {
    template<typename T> friend
    class TreeVisitor;

private:
    union {
        int _ival;
        float _fval;
        bool _bval;
    };
    std::string _sval{};

    ExpaConstant(const Token *token) : Expa(token) {
        assert(Token::Unit == token->tag);
    }

    ExpaConstant(const Token *token, int val) : Expa(token), _ival(val) {
        assert(Token::Int == token->tag);
    }

    ExpaConstant(const Token *token, float val) : Expa(token), _fval(val) {
        assert(Token::Float == token->tag);
    }

    ExpaConstant(const Token *token, bool bval) : Expa(token), _bval(bval) {
        assert(Token::Bool == token->tag);
    }

    ExpaConstant(const Token *token, const std::string &val) : Expa(token), _sval(val) {
        assert(Token::String == token->tag);
    }

public:
    ~ExpaConstant() {}

    static ExpaConstant *New(const Token *token, int val) { return new ExpaConstant(token, val); }

    static ExpaConstant *New(const Token *token, float val) { return new ExpaConstant(token, val); }

    static ExpaConstant *New(const Token *token, bool val) { return new ExpaConstant(token, val); }

    static ExpaConstant *New(const Token *token) { return new ExpaConstant(token); }

    static ExpaConstant *New(const Token *token, const std::string &val) { return new ExpaConstant(token, val); }

    virtual void Serialize(std::ostream &os);

};

/// Expa if
/*
 * expaIf ::= if exp then exp [else exp]
 * */
class ExpaIf : public Expa {
    template<typename T> friend
    class TreeVisitor;

private:
    Exp *_cond;
    Exp *_then;
    Exp *_els;

    ExpaIf(const Token *token, Exp *cond, Exp *then, Exp *els) : Expa(token), _cond(cond), _then(then), _els(els) {}

public:
    ~ExpaIf() { delete _cond, _then, _els; }

    static ExpaIf *New(const Token *token, Exp *cond, Exp *then, Exp *els = nullptr) {
        return new ExpaIf(token, cond, then, els);
    };

    virtual void Serialize(std::ostream &os);

};

/// Expa while
/*
 * expaWhile ::= while exp do exp done
* */
class ExpaWhile : public Expa {
    template<typename T> friend
    class TreeVisitor;

private:
    Exp *_cond;
    Exp *_body;

    ExpaWhile(const Token *token, Exp *cond, Exp *body) : Expa(token), _cond(cond), _body(body) {}

public:
    ~ExpaWhile() { delete _cond, _body; }

    static ExpaWhile *New(const Token *token, Exp *cond, Exp *body) { return new ExpaWhile(token, cond, body); }

    virtual void Serialize(std::ostream &os);

};

/// Expa let
/*
 * expaLet ::= let var = exp [and var = exp]* in exp
 * */
class ExpaLet : public Expa {
    template<typename T> friend
    class TreeVisitor;

private :
    ExpaLet(const Token *token) : Expa(token), expPairList(new ExpPairList) {}

public:
    Exp *body;
    ExpPairList *expPairList;

    ~ExpaLet() { delete body, expPairList; }

    static ExpaLet *New(const Token *token) {
        return new ExpaLet(token);
    }

    virtual void Serialize(std::ostream &os);

};


#endif //LEOML_PARSETREE_H
