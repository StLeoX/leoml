//
// Created by leo on 2022/4/4.
//


#include "ParseTree.h"

int ntab = 0;
#define INC ntab++
#define DEC ntab--
#define TAB for (int i = 0; i < ntab; ++i) {os<<"  ";}
#define LF os<<'\n'
#define ILT INC;LF;TAB
#define LT LF;TAB

std::ostream &operator<<(std::ostream &os, const Program &program) {
    os << "+ program";
    INC;
    for (auto decl:*program.declList) {
        LT;
        os << *decl;
    }
    DEC;
    return os;
}

std::ostream &operator<<(std::ostream &os, const Decl &decl) {
    os << "+ decl";
    ILT;
    if (decl.exp != nullptr) {
        os << *decl.exp;
        LT;
    }
    os << "+ var list";
    INC;
    for (auto var:*decl.varList) {
        LT;
        os << *var;
    }
    DEC;
    DEC;
    return os;
}

std::ostream &operator<<(std::ostream &os, const Var &var) {
    os << "| var" << var._root->str;
    return os;
}

std::ostream &operator<<(std::ostream &os, const Exp &exp) {
    os << "+ exp";
    ILT;
    if (exp.var != nullptr) {
        os << *exp.var;
        LT;
    }
    os << "+ expb list";  // todo
    INC;
    for (auto expb:*exp.expbList) {
        LT;
        os << *expb;
    }
    DEC;
    DEC;
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpbBinary &expbBinary) {
    os << "+ expbBinary";
    ILT;
    os << "| op\t" << Token::TagLookup(expbBinary._op);
    LT;
    os << "| lhs";
    LT;
    os << *expbBinary._lhs;
    LT;
    os << "| rhs";
    LT;
    os << *expbBinary._rhs;
    DEC;
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpbUnary &expbUnary) {
    os << "+ expbUnary";
    ILT;
    os << "| op\t" << Token::TagLookup(expbUnary._op);
    LT;
    os << "| oprand";
    LT;
    os << *expbUnary._oprand;
    DEC;
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpbCons &expbCons) {
    os << "+ expbCons";
    ILT;
    os << "| first element";
    LT;
    os << *expbCons._first;
    LT;
    os << "| second element";
    LT;
    os << *expbCons._second;
    DEC;
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpbCompound &expbCompound) {
    os << "+ expbCompound";
    ILT;
    os << "| first clause";
    LT;
    os << *expbCompound._first;
    LT;
    os << "| second clause";
    LT;
    os << *expbCompound._second;
    DEC;
    return os;
}

std::ostream &operator<<(std::ostream &os, const Expa &expa) {
    os << "+ expa";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpaConstant &expaConstant) {
    os << "+ expaConstant";
    ILT;
    switch (expaConstant._root->tag) {
        case Token::Int:
            os << "| value\t" << expaConstant._ival;
            break;
        case Token::Float:
            os << "| value\t" << expaConstant._fval;
            break;
        case Token::Bool:
            os << "| value\t" << expaConstant._bval;
            break;
        case Token::String:
            os << "| value\t" << expaConstant._sval;
            break;
        case Token::Unit:
            os << "| value\t" << "()";
            break;
        default:
            CompilePanic("unreachable expaConstant operator<<");
    }
    DEC;
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpaIf &expaIf) {
    os << "+ expaIf";
    ILT;
    os << "+ condition expr";
    LT;
    os << *expaIf._cond;
    LT;
    os << "+ then expr";
    LT;
    os << *expaIf._then;

    if (expaIf._els != nullptr) {
        LT;
        os << "+ else expr";
        LT;
        os << *expaIf._els;
    }
    DEC;
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpaWhile &expaWhile) {
    os << "+ expaWhile";
    ILT;
    os << "+ condition expr";
    LT;
    os << *expaWhile._cond;
    LT;
    os << "+ body expr";
    LT;
    os << *expaWhile._body;
    DEC;
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpaLet &expaLet) {
    os << "+ expaLet";
    ILT;
    os << "+ var-exp pairs";
    INC;
    for (auto item:*expaLet.varExpList) {
        LT;
        os << *item.first;
        LT;
        os << *item.second;
    }
    DEC;
    LT;
    os << "+ body expr";
    LT;
    os << *expaLet._body;
    DEC;
    return os;
}
