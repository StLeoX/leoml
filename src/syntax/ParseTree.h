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
#include "Scope.h"
#include "Type.h"

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
    Program() : stmtList(new StmtList), scope(Scope::New(nullptr, S_FILE)) {};

public:
    StmtList *stmtList;
    Scope *scope;

    ~Program() { delete stmtList, scope; };

    static Program *New() {
        return new Program();
    }

    void Serialize(std::ostream &os);

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

private:
    Stmt(Program *program) : scope(Scope::New(program->scope, S_BLOCK)) {}

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
    Scope *scope;

    ~Stmt();

    static Stmt *New(Program *program) {
        assert(program != nullptr);
        return new Stmt(program);
    }

    void Serialize(std::ostream &os);

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
    const Token *_root;
    Type *_type;

    Exp(const Token *token) : _root(token), _type(Type::New(Type::T_Unknown)), expbList(new ExpbList),
                              scope(new Scope(nullptr, S_BLOCK)) {};

public:
    Var *var;
    ExpbList *expbList;
    Scope *scope;

    virtual ~Exp() { delete _root, _type, var, expbList; };

    static Exp *New(const Token *token) { return new Exp(token); }

    virtual void Serialize(std::ostream &os);

    virtual void TypeCheck() {};

    virtual void ScopeCheck() {};

    const Token *GetRoot() const { return _root; };

    virtual Type *GetType() { return _type; }

    void SetType(Type *type) { *_type = *type; }

    void SetType(int kind) { _type->kind = kind; }

    bool IsVar() const { return _root->tag == Token::Var; }

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

    virtual void TypeCheck() {};

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

    /*
     * match(op) => AdditiveOpTypeCheck/EqualityOpTypeCheck
     * */
    virtual void TypeCheck();

    /*
     * op: +, -, *, /
     * check rule:
     *     type(lhs) == type(rhs) == T_Int/T_Float
     * infer rule:
     *     _type = T_Int/T_Float
     * */
    void AdditiveOpTypeCheck();

    /*
     * op: <, >, <=, >=, ==, !=
     * check rule:
     *     type(lhs) == type(rhs) == T_Int/T_Float/T_Bool
     *     true > false
     * infer rule:
     *     _type = T_Bool
     * */
    void EqualityOpTypeCheck();

    /*
     * op: ||, &&
     * check rule:
     *     type(lhs) == type(rhs) == T_Bool
     * infer rule:
     *     _type = T_Bool
     * */
    void BooleanOpTypeCheck();

    /*
     * Complete the scope
     * */
    virtual void ScopeCheck();

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

    /*
     * check rule:
     *     type(oprand) == T_Int/T_Float
     * infer rule:
     *     _type = T_Int/T_Float
     * */
    virtual void TypeCheck();

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

/// Expb Snd
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

protected:
    Var(const Token *token) : Expa(token) { name = token->str; }

public:
    std::string name;

    ~Var() { delete _root; };

    static Var *New(const Token *token) { return new Var(token); }

    void SetTok(const Token *token) { _root = token; }

    virtual void Serialize(std::ostream &os);

};

/// Func
/*
 * func ::= var [( varlist )]?  # varlist optional
 * func alias funcStmt, func extends var for scope.
 * */
class Func : public Var {
    template<typename T> friend
    class TreeVisitor;

protected:
    Func(const Token *token) : Var(token), paramList(new VarList), fun(new TFunc()), isRec(false),
                               scope(new Scope(nullptr, S_FUNC)) {}

public:
    Exp *body;
    VarList *paramList;
    TFunc *fun;
    bool isRec;
    Scope *scope;

    virtual ~Func() { delete body, paramList; }

    static Func *New(const Token *token) { return new Func(token); }

    virtual void Serialize(std::ostream &os);

    /*
     * check rule:
     *     None
     * infer rule:
     *     _type = T_Func
     *     fun.retType = type(body)
     *     infer paramTypeList from body, scope needed.
     * */
    virtual void TypeCheck();

    /*
     * Return fun.retType instead of this._type as the type.
     * */
    virtual Type *GetType() { return fun->retType; }

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

    /*
     * check rule:
     *     TypeCheck(arg) for arg in argList, scope needed.
     *     Preparation: find the FuncDecl in the scope, pass it as an arg.
     * infer rule:
     *     None
     * todo: notice "rec" for special scope.
     * */
    virtual void TypeCheck(Func *fund);

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
        assert(token->tag == Token::Unit);
        _type->kind = Type::T_Unit;
    }

    ExpaConstant(const Token *token, int val) : Expa(token), _ival(val) {
        assert(token->tag == Token::Int);
        _type->kind = Type::T_Int;
    }

    ExpaConstant(const Token *token, float val) : Expa(token), _fval(val) {
        assert(token->tag == Token::Float);
        _type->kind = Type::T_Float;
    }

    ExpaConstant(const Token *token, bool bval) : Expa(token), _bval(bval) {
        assert(token->tag == Token::Bool);
        _type->kind = Type::T_Bool;
    }

    ExpaConstant(const Token *token, const std::string &val) : Expa(token), _sval(val) {
        assert(token->tag == Token::String);
        _type->kind = Type::T_Unknown;
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

/// Expa If
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

    /*
     * check rule:
     *     type(cond) == T_Bool
     *     type(then) [== type(els)] == T_Int/T_Float/T_Bool
     * infer rule:
     *     _type = type(then)
     * */
    virtual void TypeCheck();

};

/// Expa While
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

    /*
     * check rule:
     *     type(cond) == T_Bool
     *     type(body) == T_Unit
     * infer rule:
     *     _type = T_Unit
     * */
    virtual void TypeCheck();

};

/// Expa Let
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

    /*
     * check rule:
     *     TypeCheck(pair) for pair in expPairList
     * infer rule:
     *     _type = type(body)
     * */
    virtual void TypeCheck();
};


#endif //LEOML_PARSETREE_H
