//
// Created by leo on 2022/4/4.
//
// Impls for serialize the ParseTreeNode
//


#include "ParseTree.h"

static int ntab = 0;

static inline void INC() { ntab++; }

static inline void DEC() { ntab--; }

static inline void TAB(std::ostream &os) { for (int i = 0; i < ntab; ++i) { os << "  "; }}

static inline void ILT(std::ostream &os) {
    ntab++;
    os << '\n';
    TAB(os);
}

static inline void LT(std::ostream &os) {
    os << '\n';
    TAB(os);
}

void Program::Serialize(std::ostream &os) {
    os << "+ program";
    INC();
    for (auto stmt:*stmtList) {
        LT(os);
        stmt->Serialize(os);
    }
    DEC();
}

void Stmt::Serialize(std::ostream &os) {
    switch (kind) {
        case FuncAssignStmt:
            func->Serialize(os);
            break;
        case VarStmt:
            os << "+ var single";
            ILT(os);
            var->Serialize(os);
            DEC();
            break;
        case VarAssignStmt:
            os << "+ var define";
            ILT(os);
            os << "+ left value";
            ILT(os);
            var->Serialize(os);
            DEC();
            LT(os);
            os << "+ right value";
            ILT(os);
            exp->Serialize(os);
            DEC();
            DEC();
            break;
        default:
            CompilePanic("unreachable");
    }
}

Stmt::~Stmt() {
    delete var, func, exp, scope;
}

void Exp::Serialize(std::ostream &os) {
    if (var != nullptr) {
        var->Serialize(os);
    }
    if (expbList == nullptr || expbList->empty()) {}
    else if (expbList->size() == 1) {
        expbList->front()->Serialize(os);
    } else {
        LT(os);
        os << "+ expb list";
        INC();
        for (auto expb:*expbList) {
            LT(os);
            expb->Serialize(os);
        }
        DEC();
    }
}

void Exp::ScopeCheck() {
    scope->Insert(var);
    for (auto expb:*expbList) {
        scope->Append(scope);
    }
}

void Func::Serialize(std::ostream &os) {
    os << "+ func define";
    os << "  name:  " << name;
    os << "  type: " << fun->GetName();
    ILT(os);
    os << "+ param list";
    INC();
    for (auto var:*paramList) {
        LT(os);
        var->Serialize(os);
    }
    DEC();
    LT(os);
    os << "+ body";
    if (body != nullptr) {
        ILT(os);
        body->Serialize(os);
        DEC();
    }
    DEC();
}

void Func::TypeCheck() {
    _type->kind = Type::T_Func;
    fun->retType->kind = body->GetType()->kind;
    for (auto param:*paramList) {
        if (Var *found = body->scope->Find(param->GetRoot())) {
            fun->paramTypeList->push_back(Type::New(found->GetType()->kind));
            param->SetType(found->GetType()->kind);
        } else {
            fun->paramTypeList->push_back(Type::New(Type::T_Unknown));
        }
    }
}

void FuncCall::Serialize(std::ostream &os) {
    os << "+ func call";
    os << "  name:  " << name;
    ILT(os);
    if (argList != nullptr) {
        os << "+ arg list";
        INC();
        for (auto expb:*argList) {
            LT(os);
            expb->Serialize(os);
        }
        DEC();
    }
    if (body != nullptr) {
        os << "+ body";
        body->Serialize(os);
    }
    if (retValue != nullptr) {
        os << "+ retValue";
        retValue->Serialize(os);
    }
    DEC();
}

void FuncCall::TypeCheck() {
    fun->retType->ExpectOrInfer(proto->GetType()->kind, _root);
    if (argList->size() != proto->paramList->size()) { CompileError(_root, "the count of arguments is unmatched"); }
    auto ap = argList->begin();
    auto pp = proto->paramList->begin();
    while (ap != argList->end() && pp != proto->paramList->end()) {
        (*ap)->GetType()->Expect((*pp)->GetType()->kind, (*ap)->GetRoot());
        ap++;
        pp++;
    }
    // after validation, then assign
    fun = proto->fun;
}

void ExpbBinary::Serialize(std::ostream &os) {
    os << "+ expbBinary";
    os << "  type: " << Type::KindLookup(_type->kind);
    ILT(os);
    os << "| op  " << Token::TagLookup(_op);
    LT(os);
    os << "+ lhs";
    ILT(os);
    _lhs->Serialize(os);
    DEC();
    LT(os);
    os << "+ rhs";
    ILT(os);
    _rhs->Serialize(os);
    DEC();
    DEC();
}

ExpbBinary::~ExpbBinary() {
    delete _lhs, _rhs;
}

void ExpbBinary::AdditiveOpTypeCheck() {
    auto ltype = _lhs->GetType();
    auto rtype = _rhs->GetType();
    switch (ltype->kind) {
        case Type::T_Unknown:
            ltype->ExpectOrInfer(Type::T_Int, _lhs->GetRoot());
            rtype->ExpectOrInfer(Type::T_Int, _rhs->GetRoot());
            _type->kind = Type::T_Int;
            break;
        case Type::T_Int:
            rtype->ExpectOrInfer(Type::T_Int, _rhs->GetRoot());
            _type->kind = Type::T_Int;
            break;
        case Type::T_Float:
            rtype->ExpectOrInfer(Type::T_Float, _rhs->GetRoot());
            _type->kind = Type::T_Float;
            break;
        default:
            Type::UnExpect(ltype->kind, _lhs->GetRoot());
            break;
    }
}

void ExpbBinary::EqualityOpTypeCheck() {
    auto ltype = _lhs->GetType();
    auto rtype = _rhs->GetType();
    _type->kind = Type::T_Bool;
    switch (ltype->kind) {
        case Type::T_Unknown:
            ltype->ExpectOrInfer(Type::T_Int, _lhs->GetRoot());
            rtype->ExpectOrInfer(Type::T_Int, _rhs->GetRoot());
            break;
        case Type::T_Int:
            rtype->ExpectOrInfer(Type::T_Int, _rhs->GetRoot());
            break;
        case Type::T_Float:
            rtype->ExpectOrInfer(Type::T_Float, _rhs->GetRoot());
            break;
        case Type::T_Bool:
            rtype->ExpectOrInfer(Type::T_Bool, _rhs->GetRoot());
            break;
        default:
            Type::UnExpect(ltype->kind, _lhs->GetRoot());
            break;
    }
}

void ExpbBinary::BooleanOpTypeCheck() {
    auto ltype = _lhs->GetType();
    auto rtype = _rhs->GetType();
    _type->kind = Type::T_Bool;
    ltype->ExpectOrInfer(Type::T_Bool, _lhs->GetRoot());
    rtype->ExpectOrInfer(Type::T_Bool, _rhs->GetRoot());
}

void ExpbBinary::TypeCheck() {
    switch (_op) {
        case '+':
        case '-':
        case '*':
        case '/':
            AdditiveOpTypeCheck();
            break;
        case '<':
        case '>':
        case Token::Ge:
        case Token::Le:
        case Token::Eq:
        case Token::Ne:
            EqualityOpTypeCheck();
            break;
        case Token::An:
        case Token::Or:
            BooleanOpTypeCheck();
            break;
        default:
            CompilePanic("unreachable");
            break;
    }
}

void ExpbBinary::ScopeCheck() {
    scope->Append(_lhs->scope);
    scope->Append(_rhs->scope);
}

void ExpbUnary::Serialize(std::ostream &os) {
    os << "+ expbUnary";
    ILT(os);
    os << "| op  " << Token::TagLookup(_op);
    LT(os);
    os << "+ oprand";
    ILT(os);
    _oprand->Serialize(os);
    DEC();
    DEC();
}

void ExpbUnary::TypeCheck() {
    switch (_op) {
        case '+':
        case '-':
            switch (_oprand->GetType()->kind) {
                case Type::T_Int:
                    _type->kind = Type::T_Int;
                    break;
                case Type::T_Float:
                    _type->kind = Type::T_Float;
                    break;
                default:
                    Type::UnExpect(_oprand->GetType()->kind, _oprand->GetRoot());
                    break;
            }
            break;
        default:
            CompilePanic("unreachable");
            break;
    }
}

void ExpbCons::Serialize(std::ostream &os) {
    os << "+ expbCons";
    ILT(os);
    os << "+ first element";
    ILT(os);
    _first->Serialize(os);
    DEC();
    LT(os);
    os << "+ second element";
    ILT(os);
    _second->Serialize(os);
    DEC();
    DEC();
}

ExpbCons::~ExpbCons() { delete _first, _second; }

void ExpbCompound::Serialize(std::ostream &os) {
    os << "+ expbCompound";
    ILT(os);
    os << "+ first clause";
    ILT(os);
    _first->Serialize(os);
    DEC();
    LT(os);
    os << "+ second clause";
    ILT(os);
    _second->Serialize(os);
    DEC();
    DEC();
}

ExpbCompound::~ExpbCompound() { delete _first, _second; }

void ExpbCompound::TypeCheck() {
    _first->GetType()->ExpectOrInfer(Type::T_Unit, _first->GetRoot());
    SetType(_second->GetType());
}

void ExpbFst::Serialize(std::ostream &os) {
    os << "+ expbFst";
    ILT(os);
    os << "+ first element";
    ILT(os);
    _first->Serialize(os);
    DEC();
    LT(os);
    os << "+ second element";
    ILT(os);
    _second->Serialize(os);
    DEC();
    DEC();
}

ExpbFst::~ExpbFst() { delete _first, _second; }

void ExpbSnd::Serialize(std::ostream &os) {
    os << "+ expbSnd";
    ILT(os);
    os << "+ first element";
    ILT(os);
    _first->Serialize(os);
    DEC();
    LT(os);
    os << "+ second element";
    ILT(os);
    _second->Serialize(os);
    DEC();
    DEC();
}

ExpbSnd::~ExpbSnd() { delete _first, _second; }

void Var::Serialize(std::ostream &os) {
    os << "| var";
    os << "  name: " << name;
    os << "  type: " << _type->GetName();
}

void ExpaConstant::Serialize(std::ostream &os) {
    os << "| expaConstant  type: " << _type->GetName() << "  value:  ";
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
    ILT(os);
    os << "+ condition expr";
    LT(os);
    _cond->Serialize(os);
    LT(os);
    os << "+ then expr";
    LT(os);
    _then->Serialize(os);

    if (_els != nullptr) {
        LT(os);
        os << "+ else expr";
        LT(os);
        _els->Serialize(os);
    }
    DEC();
}

void ExpaIf::TypeCheck() {
    _cond->GetType()->ExpectOrInfer(Type::T_Bool, _cond->GetRoot());
    if (_els == nullptr) {
        switch (_then->GetType()->kind) {
            case Type::T_Unknown:
                _then->GetType()->ExpectOrInfer(Type::T_Unit, _then->GetRoot());
                _type->kind = Type::T_Unit;
                break;
            case Type::T_Int:
                _type->kind = Type::T_Int;
                break;
            case Type::T_Float:
                _type->kind = Type::T_Float;
                break;
            case Type::T_Bool:
                _type->kind = Type::T_Bool;
                break;
            case Type::T_Unit:
                _type->kind = Type::T_Unit;
                break;
            default:
                Type::UnExpect(_then->GetType()->kind, _then->GetRoot());
                break;
        }
    } else {
        auto ttype = _then->GetType();
        auto etype = _els->GetType();
        switch (ttype->kind) {
            case Type::T_Unknown:
                ttype->ExpectOrInfer(Type::T_Unit, _then->GetRoot());
                etype->ExpectOrInfer(Type::T_Unit, _els->GetRoot());
                _type->kind = Type::T_Unit;
                break;
            case Type::T_Int:
                etype->ExpectOrInfer(Type::T_Int, _els->GetRoot());
                _type->kind = Type::T_Int;
                break;
            case Type::T_Float:
                etype->ExpectOrInfer(Type::T_Float, _els->GetRoot());
                _type->kind = Type::T_Float;
                break;
            case Type::T_Bool:
                etype->ExpectOrInfer(Type::T_Bool, _els->GetRoot());
                _type->kind = Type::T_Bool;
                break;
            case Type::T_Unit:
                etype->ExpectOrInfer(Type::T_Unit, _els->GetRoot());
                _type->kind = Type::T_Unit;
                break;
            default:
                Type::UnExpect(_then->GetType()->kind, _then->GetRoot());
                break;
        }
    }
}

void ExpaIf::ScopeCheck() {
    // todo: Here exists a Bug. Typecheck didn't influent the inner scope.
    // naive solution: only for "if a then b else c" this case.
    _cond->scope->Find(_cond->GetRoot())->SetType(_cond->GetType());
    scope->Append(_cond->scope);
    _then->scope->Find(_then->GetRoot())->SetType(_then->GetType());
    scope->Append(_then->scope);
    if (_els != nullptr) {
        _els->scope->Find(_els->GetRoot())->SetType(_els->GetType());
        scope->Append(_els->scope);
    }
}

void ExpaWhile::Serialize(std::ostream &os) {
    os << "+ expaWhile";
    ILT(os);
    os << "+ condition expr";
    LT(os);
    _cond->Serialize(os);
    LT(os);
    os << "+ body";
    LT(os);
    _body->Serialize(os);
    DEC();
}

void ExpaWhile::TypeCheck() {
    _cond->GetType()->ExpectOrInfer(Type::T_Bool, _cond->GetRoot());
    switch (_body->GetType()->kind) {
        case Type::T_Unknown:
            _body->GetType()->ExpectOrInfer(Type::T_Unit, _body->GetRoot());
            _type->kind = Type::T_Unit;
            break;
        case Type::T_Int:
            _type->kind = Type::T_Int;
            break;
        case Type::T_Float:
            _type->kind = Type::T_Float;
            break;
        case Type::T_Bool:
            _type->kind = Type::T_Bool;
            break;
        case Type::T_Unit:
            _type->kind = Type::T_Unit;
            break;
        default:
            Type::UnExpect(_body->GetType()->kind, _body->GetRoot());
            break;
    }
}

void ExpaWhile::ScopeCheck() {
    // naive solution: only for "while a do b done" this case.
    _cond->scope->Find(_cond->GetRoot())->SetType(_cond->GetType());
    scope->Append(_cond->scope);
    _body->scope->Find(_body->GetRoot())->SetType(_body->GetType());
    scope->Append(_body->scope);
}

void ExpaLet::Serialize(std::ostream &os) {
    os << "+ expaLet";
    ILT(os);
    os << "+ exp pairs";
    INC();
    for (auto item:*expPairList) {
        LT(os);
        item.first->Serialize(os);
        if (item.second != nullptr) {
            LT(os);
            item.second->Serialize(os);
        }
    }
    DEC();
    LT(os);
    os << "+ body";
    ILT(os);
    body->Serialize(os);
    DEC();
    DEC();
}

void ExpaLet::TypeCheck() {
    // todo: scope combine
    SetType(body->GetType());
}
