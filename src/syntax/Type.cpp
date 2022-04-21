//
// Created by leo on 2022/4/19.
//


#include "Type.h"

const std::unordered_map<int, const char *> Type::KindMap{
        {T_Int,     "int"},
        {T_Float,   "float"},
        {T_Bool,    "bool"},
        {T_Unit,    "unit"},
        {T_Func,    "fun"},
        {T_Unknown, "unknown"},
};

std::string Type::KindLookup(int k) {
    auto ret = KindMap.find(k);
    if (ret == KindMap.end()) {
        CompilePanic("unreachable");
        return nullptr;
    }
    return ret->second;
}

void Type::operator=(const Type &rhs) {
    kind = rhs.kind;
}

bool Type::operator==(const Type &rhs) const {
    return kind == rhs.kind;
}

bool Type::operator!=(const Type &rhs) const {
    return !(rhs == *this);
}

bool TFunc::operator==(const TFunc &rhs) const {
    if (retType != rhs.retType) {
        return false;
    }
    if (paramTypeList->size() != rhs.paramTypeList->size()) { return false; }
    auto lp = paramTypeList->begin();
    auto rp = rhs.paramTypeList->begin();
    while (lp != paramTypeList->end() && rp != rhs.paramTypeList->end()) {
        if (*(*lp) != *(*rp)) { return false; }
        lp++;
        rp++;
    }
    return true;
}

bool TFunc::operator!=(const TFunc &rhs) const {
    return !(rhs == *this);
}
