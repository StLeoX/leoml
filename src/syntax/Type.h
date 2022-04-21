//
// Created by leo on 2022/4/19.
//


#ifndef LEOML_TYPE_H
#define LEOML_TYPE_H

#include <iostream>
#include <typeinfo>
#include <unordered_map>
#include <list>

#include "Scope.h"
#include "error.hpp"


/// Type
// simple type
class Type {
private:
    static const std::unordered_map<int, const char *> KindMap;

protected:
    Type(int k) : kind(k) {};

public:
    // RTTI
    enum {
        T_Int = 1,
        T_Float,
        T_Bool,
        T_Unit,
        T_Func,
        T_Unknown = -1,
    };

    int kind;

    static std::string KindLookup(int k);

    std::string GetName() { return KindLookup(kind); }

    bool Expect(int expect, const Token *token) {
        if (kind != expect) {
            CompileError(token, "Type `%s` expected, but got `%s`",
                         Type::KindLookup(expect).c_str(),
                         Type::KindLookup(kind).c_str());
            return false;
        }
        return true;
    }

    static void UnExpect(int unexpect, const Token *token) {
        CompileError(token, "Unexpected type `%s` found here", Type::KindLookup(unexpect).c_str());
    }

    static Type *New(int k) { return new Type(k); }

    void operator=(const Type &rhs);

    bool operator==(const Type &rhs) const;

    bool operator!=(const Type &rhs) const;
};

using TypeList = std::list<Type *>;

/// TFunc
// func type IS-A compound type.
class TFunc {
private:
    TFunc() : retType(Type::New(Type::T_Unknown)), paramTypeList(new TypeList) {};

public:
    Type *retType;
    TypeList *paramTypeList;

    bool operator==(const TFunc &rhs) const;

    bool operator!=(const TFunc &rhs) const;

    std::string GetName() {
        std::string ret = "";
        if (paramTypeList->empty()) { return ret; }
        ret += paramTypeList->front()->GetName();
        paramTypeList->pop_front();
        while (!paramTypeList->empty()) {
            ret += " * " + paramTypeList->front()->GetName();
            paramTypeList->pop_front();
        }
        ret += " -> " + retType->GetName();
        return ret;
    }

};

#endif //LEOML_TYPE_H
