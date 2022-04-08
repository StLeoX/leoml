//
// Created by leo on 2022/4/4.
//
// The Abstraction of ParseTree
//

#ifndef LEOML_PARSETREE_H
#define LEOML_PARSETREE_H

#include <list>
#include <ostream>
#include "Visitor.h"
#include "Token.h"

/// AST Node Interface
/*
 * Interface:
 *     - Accept;
 *     - <<;
 * */
class ParseTreeNode {
public:
    virtual ~ParseTreeNode() {};

    virtual void Accept(Visitor *v) = 0;  // Terminate the visiting.

    virtual friend std::ostream &operator<<(std::ostream &os, const ParseTreeNode &node) = 0;

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
class Program : public ParseTreeNode {
private:
    Program() {};

public:
    DeclList declList{};

    virtual ~Program() {};

    static Program *New() {
        return new Program();
    }

};

/// Decl
class Decl : public ParseTreeNode {
    template<typename T> friend
    class TreeVisitor;

protected:
    Decl() {};

public:
    Exp *exp;
    VarList varList{};

    virtual ~Decl() {};

    static Decl *New() { return new Decl(); };

    virtual void Accept(Visitor *visitor);

};

/// Var
class Var : public ParseTreeNode {
    template<typename T> friend
    class TreeVisitor;

    friend class Expa;

private:
    const Token *_root;
public:
    ~Var() {};

    static Var *New(const Token *root) { return new Var(); }

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

    friend class Decl;

protected:
    const Token *_root;

    Exp(const Token *token) : _root(token) {};

public:
    Var *var;
    ExpList expbList{};

    virtual ~Exp() {};

    static Exp *New(const Token *token) { return new Exp(token); }

    const Token *GetToken() { return _root; }
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
};

/// Expb Binary
/*
 * expb ::= expb BinaryOp expb
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

    ExpbBinary(const Token *root, int op, Expb *lhs, Expb *rhs) : Expb(root), _lhs(lhs), _rhs(rhs), _op(op) {
    }

public:
    ~ExpbBinary() {};

    static ExpbBinary *New(const Token *token, Expb *lhs, Expb *rhs) {
        return new ExpbBinary(token, token->tag, lhs, rhs);
    }

    static ExpbBinary *New(const Token *token, int op, Expb *lhs, Expb *rhs) {
        return new ExpbBinary(token, op, lhs, rhs);
    };

};

/// Expb Unary
/*
 * expb ::= UnaryOp ( expb )
 * UnaryOp:
 *     fst, snd
 * */
class ExpbUnary : public Expb {
    template<typename T> friend
    class TreeVisitor;

private:
    Expb *_oprand;
    int _op;

    ExpbUnary(const Token *root, int op, Expb *oprand) : Expb(root), _oprand(oprand) {}

public:
    ~ExpbUnary() {};

    static ExpbUnary *New(const Token *token, Expb *oprand) { return new ExpbUnary(token, token->tag, oprand); }

    static ExpbUnary *New(const Token *token, int op, Expb *oprand) { return new ExpbUnary(token, op, oprand); };

    virtual void Accept(Visitor *visitor);
};

/// Expb Cons
/*
 * exp ::= ( first, second )
 * */
class ExpbCons : public Expb {
    template<typename T> friend
    class TreeVisitor;

private:
    Expb *_first;
    Expb *_second;

    ExpbCons(const Token *token, Expb *first, Expb *second) : Expb(token), _first(first), _second(second) {}

public:
    static ExpbCons *New(const Token *token, Expb *first, Expb *second) { return new ExpbCons(token, first, second); }
};

/// Expb Compound
/*
 * expb ::= expb; expb
 * */
class ExpbCompound : public Expb {
    template<typename T> friend
    class TreeVisitor;

private:
    Expb *_first;
    Expb *_second;

    ExpbCompound(const Token *root, int op, Expb *lhs, Expb *rhs) : Expb(root), _first(lhs), _second(rhs) {
        assert(';' == op);
    }

public:
    static ExpbCompound *New(const Token *token, Expb *lhs, Expb *rhs) {
        return new ExpbCompound(token, token->tag, lhs, rhs);
    }

};

/// Expa
/*
 * expa ::= var
 *        | expaConstant
 *        | expaIf
 *        | expaWhile
 *        | expaLet
 * */
class Expa : public Exp {
    template<typename T> friend
    class TreeVisitor;

protected:
    Expa(const Token *token) : Exp(token) {}

public:
    virtual void Accept(Visitor *visitor);

    static Expa *New(Var *var) { return new Expa(var->_root); }

    static Expa *New(Exp *exp) { return new Expa(exp->_root); }
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
    static ExpaConstant *New(const Token *token, int val) { return new ExpaConstant(token, val); }

    static ExpaConstant *New(const Token *token, float val) { return new ExpaConstant(token, val); }

    static ExpaConstant *New(const Token *token, bool val) { return new ExpaConstant(token, val); }

    static ExpaConstant *New(const Token *token) { return new ExpaConstant(token); }

    static ExpaConstant *New(const Token *token, const std::string &val) { return new ExpaConstant(token, val); }

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
    static ExpaIf *New(const Token *token, Exp *cond, Exp *then, Exp *els = nullptr) {
        return new ExpaIf(token, cond, then, els);
    };
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
    static ExpaWhile *New(const Token *token, Exp *cond, Exp *body) { return new ExpaWhile(token, cond, body); }
};

/// Expa let
/*
 * expaLet ::= let var = exp [and var = exp]* in exp
 * */
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
