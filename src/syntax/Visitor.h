//
// Created by leo on 2022/4/5.
//
// Visitor used by Parser.
// The Abstraction of Visiting the AST.
//

#ifndef LEOML_VISITOR_H
#define LEOML_VISITOR_H

#include "ParseTree.h"
#include "error.hpp"

class Visitor {
public:
    virtual ~Visitor() {};

private:
    virtual void VisitProgram(Program *program) = 0;

    virtual void VisitDecl(Decl *decl) = 0;

    virtual void VisitVar(Var *var) = 0;

    virtual void VisitExp(Exp *exp) = 0;

    virtual void VisitExpb(Expb *expb) = 0;

    virtual void VisitExpbBinary(ExpbBinary *expbBinary) = 0;

    virtual void VisitExpbUnary(ExpbUnary *expbUnary) = 0;

    virtual void VisitExpbCompound(ExpbCompound *expbCompound) = 0;

    virtual void VisitExpa(Expa *expa) = 0;

    virtual void VisitExpaIf(Expa *expaIf) = 0;

    virtual void VisitExpaWhile(Expa *expaWhile) = 0;

};

template<typename T>
class TreeVisitor : public Visitor {
public:
    TreeVisiteor() {}

    ~TreeVisitor() {}

    // main API
    virtual void VisitProgram(Program *program);

private:
    T _val;

    T Eval(Exp *exp) {
        exp->Accept(this);
        return _val;
    }

    virtual void VisitDecl(Decl *decl);

    virtual void VisitVar(Var *var);

    virtual void VisitExp(Exp *exp);

    virtual void VisitExpb(Expb *expb);

    virtual void VisitExpbBinary(ExpbBinary *expbBinary);

    virtual void VisitExpbUnary(ExpbUnary *expbUnary);

    virtual void VisitExpbCompound(ExpbCompound *expbCompound);

    virtual void VisitExpa(Expa *expa);

    virtual void VisitExpaIf(Expa *expaIf);

    virtual void VisitExpaWhile(Expa *expaWhile);
};


#endif //LEOML_VISITOR_H
