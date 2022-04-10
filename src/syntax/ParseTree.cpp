//
// Created by leo on 2022/4/4.
//
// Impls for serialize the ParseTreeNode
//


#include "ParseTree.h"

int ntab = 0;
#define INC ntab++
#define DEC ntab--
#define TAB for (int i = 0; i < ntab; ++i) {os<<"  ";}
#define LF os<<'\n'
#define ILT INC;LF;TAB
#define LT LF;TAB

void Program::Serialize(std::ostream &os) {
    os << "+ program";
    INC;
    for (auto decl:*declList) {
        LT;
        decl->Serialize(os);
    }
    DEC;
}

void Decl::Serialize(std::ostream &os) {
    os << "+ decl";
    ILT;
    if (exp != nullptr) {
        exp->Serialize(os);
        LT;
    }
    os << "+ var list";
    INC;
    for (auto var:*varList) {
        LT;
        var->Serialize(os);
    }
    DEC;
    DEC;
}

void Var::Serialize(std::ostream &os) {
    os << "| var" << _root->str;
}

void Exp::Serialize(std::ostream &os) {
    os << "+ exp";
    ILT;
    if (var != nullptr) {
        var->Serialize(os);
        LT;
    }
    os << "+ expb list";  // todo
    INC;
    for (auto expb:*expbList) {
        LT;
        expb->Serialize(os);
    }
    DEC;
    DEC;
}

void ExpbBinary::Serialize(std::ostream &os) {
    os << "+ expbBinary";
    ILT;
    os << "| op\t" << Token::TagLookup(_op);
    LT;
    os << "| lhs";
    LT;
    _lhs->Serialize(os);
    LT;
    os << "| rhs";
    LT;
    _rhs->Serialize(os);
    DEC;
}

void ExpbUnary::Serialize(std::ostream &os) {
    os << "+ expbUnary";
    ILT;
    os << "| op\t" << Token::TagLookup(_op);
    LT;
    os << "| oprand";
    LT;
    _oprand->Serialize(os);
    DEC;
}

void ExpbCons::Serialize(std::ostream &os) {
    os << "+ expbCons";
    ILT;
    os << "| first element";
    LT;
    _first->Serialize(os);
    LT;
    os << "| second element";
    LT;
    _second->Serialize(os);
    DEC;
}

void ExpbCompound::Serialize(std::ostream &os) {
    os << "+ expbCompound";
    ILT;
    os << "| first clause";
    LT;
    _first->Serialize(os);
    LT;
    os << "| second clause";
    LT;
    _second->Serialize(os);
    DEC;
}

void ExpaConstant::Serialize(std::ostream &os) {
    os << "+ expaConstant";
    ILT;
    switch (_root->tag) {
        case Token::Int:
            os << "| value\t" << _ival;
            break;
        case Token::Float:
            os << "| value\t" << _fval;
            break;
        case Token::Bool:
            os << "| value\t" << _bval;
            break;
        case Token::String:
            os << "| value\t" << _sval;
            break;
        case Token::Unit:
            os << "| value\t" << "()";
            break;
        default:
            CompilePanic("unreachable expaConstant operator<<");
    }
    DEC;
}

void ExpaIf::Serialize(std::ostream &os) {
    os << "+ expaIf";
    ILT;
    os << "+ condition expr";
    LT;
    _cond->Serialize(os);
    LT;
    os << "+ then expr";
    LT;
    _then->Serialize(os);

    if (_els != nullptr) {
        LT;
        os << "+ else expr";
        LT;
        _els->Serialize(os);
    }
    DEC;
}

void ExpaWhile::Serialize(std::ostream &os) {
    os << "+ expaWhile";
    ILT;
    os << "+ condition expr";
    LT;
    _cond->Serialize(os);
    LT;
    os << "+ body expr";
    LT;
    _body->Serialize(os);
    DEC;
}

void ExpaLet::Serialize(std::ostream &os) {
    os << "+ expaLet";
    ILT;
    os << "+ var-exp pairs";
    INC;
    for (auto item:*varExpList) {
        LT;
        item.first->Serialize(os);
        LT;
        item.second->Serialize(os);
    }
    DEC;
    LT;
    os << "+ body expr";
    LT;
    _body->Serialize(os);
    DEC;
}
