//
// Created by leo on 2022/4/19.
//


#include "syntax/Scope.h"
#include "syntax/ParseTree.h"

Var *Scope::Find(const Token *token) {
    auto ret = Find(token->str);
    if (ret) ret->SetTok(token);
    return ret;
}

Var *Scope::FindInCurScope(const Token *token) {
    auto ret = FindInCurScope(token->str);
    if (ret) ret->SetTok(token);
    return ret;
}

Var *Scope::FindTag(const Token *token) {
    auto ret = FindTag(token->str);
    if (ret) ret->SetTok(token);
    return ret;
}

Var *Scope::FindTagInCurScope(const Token *token) {
    auto ret = FindTagInCurScope(token->str);
    if (ret) ret->SetTok(token);
    return ret;
}

void Scope::Insert(Var *var) {
    Insert(var->name, var);
}

void Scope::InsertTag(Var *var) {
    Insert(TagName(var->name), var);
}

void Scope::Append(Scope *other) {
    for (auto &item:other->_map) {
        _map.insert(item);
    }
}

Var *Scope::Find(const std::string &name) {
    auto var = _map.find(name);
    if (var != _map.end())
        return var->second;
    if (_kind == S_FILE || _parent == nullptr)
        return nullptr;
    return _parent->Find(name);
}

Var *Scope::FindInCurScope(const std::string &name) {
    auto var = _map.find(name);
    if (var == _map.end())
        return nullptr;
    return var->second;
}

void Scope::Insert(const std::string &name, Var *var) {
    assert(var != nullptr);
    _map[name] = var;
}

Var *Scope::FindTag(const std::string &name) {
    auto ret = Find(TagName(name));
    return ret;
}

Var *Scope::FindTagInCurScope(const std::string &name) {
    auto ret = FindInCurScope(TagName(name));
    return ret;
}

Scope::TagList Scope::AllTagsInCurScope() const {
    TagList ret;
    for (auto &kv: _map) {
        if (IsTagName(kv.first))
            ret.push_back(kv.second);
    }
    return ret;
}

void Scope::Serialize(std::ostream &os) {
    os << "scope: " << this << std::endl;
    auto iter = _map.begin();
    for (; iter != _map.end(); ++iter) {
        auto name = iter->first;
        auto var = iter->second;
        if (var != nullptr) {
            os << name << "\t[type:\t"
               << var->GetType()->GetName() << "]" << std::endl;
        } else {
            os << name << "\t[object:\t"
               << var->GetType()->GetName() << "]" << std::endl;
        }
    }
    std::cout << std::endl;
}
