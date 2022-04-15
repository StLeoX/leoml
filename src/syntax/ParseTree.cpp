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
    switch (kind) {
        case FuncDecl:
            func->Serialize(os);
            break;
        case VarDecl:
            os << "+ var decl";
            ILT;
            var->Serialize(os);
            DEC;
            break;
        case AssignDecl:
            os << "+ assign decl";
            ILT;
            os << "+ left value";
            ILT;
            var->Serialize(os);
            DEC;
            LT;
            os << "+ right value";
            ILT;
            exp->Serialize(os);
            DEC;
            DEC;
            break;
        default:
            CompilePanic("unreachable");
    }
}

Decl::~Decl() {
    delete var, func;
}

void Exp::Serialize(std::ostream &os) {
    if (var != nullptr) {
        var->Serialize(os);
        LT;
    }
    if (expbList == nullptr || expbList->empty()) {}
    else if (expbList->size() == 1) {
        expbList->front()->Serialize(os);
    } else {
        os << "+ expb list";
        INC;
        for (auto expb:*expbList) {
            LT;
            expb->Serialize(os);
        }
        DEC;
    }
}

void Func::Serialize(std::ostream &os) {
    os << "+ func decl";
    os << "  name:  " << name->GetStr();
    ILT;
    os << "+ arg list";
    INC;
    for (auto var:*argList) {
        LT;
        var->Serialize(os);
    }
    DEC;
    LT;
    os << "+ body";
    if (body != nullptr) {
        ILT;
        body->Serialize(os);
        DEC;
    }
    DEC;
}

void FuncCall::Serialize(std::ostream &os) {
    os << "+ func call";
    os << "  name:  " << name->GetStr();
    ILT;
    os << "+ arg list";
    INC;
    for (auto var:*argList) {
        LT;
        var->Serialize(os);
    }
    DEC;
    if (body != nullptr) {
        LT;
        os << "+ body";
        body->Serialize(os);
    }
    if (retValue != nullptr) {
        LT;
        os << "+ retValue";
        retValue->Serialize(os);
    }
    DEC;
}

void ExpbBinary::Serialize(std::ostream &os) {
    os << "+ expbBinary";
    ILT;
    os << "| op  " << Token::TagLookup(_op);
    LT;
    os << "+ lhs";
    ILT;
    _lhs->Serialize(os);
    DEC;
    LT;
    os << "+ rhs";
    ILT;
    _rhs->Serialize(os);
    DEC;
    DEC;
}

ExpbBinary::~ExpbBinary() {
    delete _lhs, _rhs;
}

void ExpbUnary::Serialize(std::ostream &os) {
    os << "+ expbUnary";
    ILT;
    os << "| op  " << Token::TagLookup(_op);
    LT;
    os << "+ oprand";
    ILT;
    _oprand->Serialize(os);
    DEC;
    DEC;
}

void ExpbCons::Serialize(std::ostream &os) {
    os << "+ expbCons";
    ILT;
    os << "+ first element";
    ILT;
    _first->Serialize(os);
    DEC;
    LT;
    os << "+ second element";
    ILT;
    _second->Serialize(os);
    DEC;
    DEC;
}

ExpbCons::~ExpbCons() { delete _first, _second; }

void ExpbCompound::Serialize(std::ostream &os) {
    os << "+ expbCompound";
    ILT;
    os << "+ first clause";
    ILT;
    _first->Serialize(os);
    DEC;
    LT;
    os << "+ second clause";
    ILT;
    _second->Serialize(os);
    DEC;
    DEC;
}

ExpbCompound::~ExpbCompound() { delete _first, _second; }

void ExpbFst::Serialize(std::ostream &os) {
    os << "+ expbFst";
    ILT;
    os << "+ first element";
    ILT;
    _first->Serialize(os);
    DEC;
    LT;
    os << "+ second element";
    ILT;
    _second->Serialize(os);
    DEC;
    DEC;
}

ExpbFst::~ExpbFst() { delete _first, _second; }

void ExpbSnd::Serialize(std::ostream &os) {
    os << "+ expbSnd";
    ILT;
    os << "+ first element";
    ILT;
    _first->Serialize(os);
    DEC;
    LT;
    os << "+ second element";
    ILT;
    _second->Serialize(os);
    DEC;
    DEC;
}

ExpbSnd::~ExpbSnd() { delete _first, _second; }

void Var::Serialize(std::ostream &os) {
    os << "| var  ident: " << _root->str;
}

void ExpaConstant::Serialize(std::ostream &os) {
    os << "| expaConstant  type: " << Token::TagLookup(_root->tag) << "  value:  ";
    switch (_root->tag) {
        case Token::Int:
            os << _ival;
            break;
        case Token::Float:
            os << _fval;
            break;
        case Token::Bool:
            os << _bval ? "true" : "false";
            break;
        case Token::String:
            os << _sval;
            break;
        case Token::Unit:
            os << "()";
            break;
        default:
            CompilePanic("unreachable expaConstant operator<<");
    }
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
