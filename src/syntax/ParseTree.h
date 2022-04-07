//
// Created by leo on 2022/4/4.
//
// The Abstraction of ParseTree
//

#ifndef LEOML_PARSETREE_H
#define LEOML_PARSETREE_H

#include <list>
#include "Visitor.h"
#include "Token.h"

/// AST Node
class ASTNode {
public:
    virtual ~ASTNode() {}

    virtual void Accept(Visitor *v) = 0;  // Terminate the visiting.

protected:
    ASTNode() {}
};

/// Language Hierarchy Model
class Decl;  // Decl

using DeclList = std::list<Decl *>; // DeclList

class Var;

using VarList = std::list<Var *>; // Varlist

class Exp; // Exp

using ExpList = std::list<Exp *>; // Explist

using VarExpList = std::list<std::pair<Var *, Exp *>>;

class Expb; //Expb

class ExpbBinary;

class ExpbUnary;

class ExpbCompound;

class Expa; //Expa

class ExpaIf;

class ExpaWhile;

class ExpaLet;


/// Program
class Program : public ASTNode {
private:
    Program() : decllist{} {};

public:
    static Program *New() {
        return new Program();
    }

    virtual ~Program() {};
    DeclList decllist;

};

/// Decl
class Decl : public ASTNode {
    template<typename T> friend
    class TreeVisitor;

protected:
    Decl() {};

public:
    VarList varList{};

    static Decl *New() { return new Decl(); };

    virtual ~Decl() {};

    virtual void Accept(Visitor *visitor);

};


/// Var
class Var : public ASTNode {
    template<typename T> friend
    class TreeVisitor;

public:
    static Var *New(const Token *root) { return new Var(); }

    ~Var() {};
};

/// Exp
class Exp : public ASTNode {
    template<typename T> friend
    class TreeVisitor;

    friend class Decl;

protected:
    const Token *_root;

    Exp(const Token *token) : _root(token) {};

public:
    virtual ~Exp() {};

    const Token *GetToken() { return _root; }
};

/// Expb
class Expb : public Exp {
    template<typename T> friend
    class TreeVisitor;

protected:
    Expb(const Token *token) : Exp(token) {};

public:
    virtual ~Expb() {}

};

/// Expb Binary
/*
 * Binary Operations:
 *     +, -, *, /, <, <=, ==, !=, >, >=, ;
 *     cons,
 * */
class ExpbBinary : public Expb {
    template<typename T> friend
    class TreeVisitor;

private:
    Expb *_lhs;
    Expb *_rhs;
    int _op;

    ExpbBinary(const Token *root, int op, Expb *lhs, Expb *rhs) : Expb(root), _lhs(lhs), _rhs(rhs), _op(op) {
    }

public:
    static ExpbBinary *New(const Token *token, Expb *lhs, Expb *rhs) {
        return new ExpbBinary(token, token->tag, lhs, rhs);
    }

    static ExpbBinary *New(const Token *token, int op, Expb *lhs, Expb *rhs) {
        return new ExpbBinary(token, op, lhs, rhs);
    };

    virtual ~ExpbBinary() {};
};

/// Expb Unary
/*
 * Unary Operations:
 *     car, cdr, empty
 * */
class ExpbUnary : public Expb {
    template<typename T> friend
    class TreeVisitor;

private:
    Expb *_oprand;
    int _op;

    ExpbUnary(const Token *root, int op, Expb *oprand) : Expb(root), _oprand(oprand) {}

public:
    static ExpbUnary *New(const Token *token, Expb *oprand) { return new ExpbUnary(token, token->tag, oprand); }

    static ExpbUnary *New(const Token *token, int op, Expb *oprand) { return new ExpbUnary(token, op, oprand); };

    virtual ~ExpbUnary() {};

    virtual void Accept(Visitor *visitor);
};

/// Expb Compound
/*
 * Compound Expb ::= Expb; Expb
 * */
class ExpbCompound : public Expb {
    template<typename T> friend
    class TreeVisitor;

private:
    Expb *_lhs;
    Expb *_rhs;

    ExpbCompound(const Token *root, int op, Expb *lhs, Expb *rhs) : Expb(root), _lhs(lhs), _rhs(rhs) {
        assert(';' == op);
    }

public:
    static ExpbCompound *New(const Token *token, Expb *lhs, Expb *rhs) {
        return new ExpbCompound(token, token->tag, lhs, rhs);
    }

};

/// Expa
class Expa : public Exp {
    template<typename T> friend
    class TreeVisitor;

protected:
    Expa(const Token *token) : Exp(token) {}

public:
    virtual void Accept(Visitor *visitor);

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
        const std::string &_sval = "";
    };

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
    static ExpaConstant *New(const Token *token, int val) { return new ExpaConstant(token, val); }

    static ExpaConstant *New(const Token *token, float val) { return new ExpaConstant(token, val); }

    static ExpaConstant *New(const Token *token, bool val) { return new ExpaConstant(token, val); }

    static ExpaConstant *New(const Token *token, const std::string &val) { return new ExpaConstant(token, val); }

public:
};


/// Expa if
class ExpaIf : public Expa {
    template<typename T> friend
    class TreeVisitor;

private:
    Exp *_cond;
    Exp *_then;
    Exp *_els;

    ExpaIf(const Token *token, Exp *cond, Exp *then, Exp *els) : Expa(token), _cond(cond), _then(then), _els(els) {}

public:
    static ExpaIf *New(const Token *token, Exp *cond, Exp *then, Exp *els = nullptr) {
        return new ExpaIf(token, cond, then, els);
    };
};

/// Expa while
class ExpaWhile : public Expa {
    template<typename T> friend
    class TreeVisitor;

private:
    Exp *_cond;
    Exp *_body;

    ExpaWhile(const Token *token, Exp *cond, Exp *body) : Expa(token), _cond(cond), _body(body) {}

public:
    static ExpaWhile *New(const Token *token, Exp *cond, Exp *body) { return new ExpaWhile(token, cond, body); }
};

/// Expa let
class ExpaLet : public Expa {
    template<typename T> friend
    class TreeVisitor;

private :
    Exp *_body;

    ExpaLet(const Token *token, Exp *body, const VarExpList &varExpList) : Expa(token), _body(body),
                                                                           varExpList(varExpList) {}

public:
    VarExpList varExpList;

    static ExpaLet *New(const Token *token, Exp *body, const VarExpList &varExpList) {
        return new ExpaLet(token, body, varExpList);
    }

};


#endif //LEOML_PARSETREE_H
