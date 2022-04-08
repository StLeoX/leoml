//
// Created by leo on 2022/4/4.
//


#include "ParseTree.h"

std::ostream &operator<<(std::ostream &os, const Program &program) {
    for (auto decl:program.declList) os << decl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const Decl &decl) {
    os << "Decl: " << "Exp: " << decl.exp;
    for (auto var:decl.varList) os << var;
    return os;
}

std::ostream &operator<<(std::ostream &os, const Var &var) {
    os << "Var: " << var._root->str;
    return os;
}

std::ostream &operator<<(std::ostream &os, const Exp &exp) {
    os << "Decl: " << "\tVar: " << exp.var;
    for (auto expb:exp.expbList) {
        os << "\t" << expb;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Expb &expb) {
    os << "Expb: " << expb._root->str;
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpbBinary &expbBinary) {
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpbUnary &expbUnary) {
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpbCons &expbCons) {
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpbCompound &expbCompound) {
    return os;
}

std::ostream &operator<<(std::ostream &os, const Expa &expa) {
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpaConstant &expaConstant) {
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpaIf &expaIf) {
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpaWhile &expaWhile) {
    return os;
}

std::ostream &operator<<(std::ostream &os, const ExpaLet &expaLet) {
    return os;
}








