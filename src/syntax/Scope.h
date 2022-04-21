//
// Created by leo on 2022/4/19.
//


#ifndef LEOML_SCOPE_H
#define LEOML_SCOPE_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cassert>

class Var;

class Token;

typedef enum {
    S_FILE,
    S_BLOCK,
    S_FUNC,
} ScopeKind;

class Scope {
    using TagList = std::vector<Var *>;
    using VarMap = std::map<std::string, Var *>;

public:
    explicit Scope(Scope *parent, ScopeKind type)
            : _parent(parent), _kind(type) {}

    ~Scope() {}

    Scope *Parent() { return _parent; }

    void SetParent(Scope *parent) { _parent = parent; }

    ScopeKind Type() const { return _kind; }

    Var *Find(const Token *token);

    Var *FindInCurScope(const Token *token);

    Var *FindTag(const Token *token);

    Var *FindTagInCurScope(const Token *token);

    TagList AllTagsInCurScope() const;

    void Insert(Var *var);

    void Insert(const std::string &name, Var *var);

    void InsertTag(Var *var);

    void Serialize(std::ostream &os);

    bool operator==(const Scope &other) const { return _kind == other._kind; }

    VarMap::iterator begin() { return _map.begin(); }

    VarMap::iterator end() { return _map.end(); }

    size_t size() const { return _map.size(); }

private:
    Scope *_parent;

    ScopeKind _kind;

    VarMap _map;

    Scope(const Scope &scope);

    Var *Find(const std::string &name);

    Var *FindInCurScope(const std::string &name);

    Var *FindTag(const std::string &name);

    Var *FindTagInCurScope(const std::string &name);

    std::string TagName(const std::string &name) {
        return name + "@:tag";
    }

    static bool IsTagName(const std::string &name) {
        return name.size() > 5 && name[name.size() - 5] == '@';
    }

    const Scope &operator=(const Scope &other);

};

#endif //LEOML_SCOPE_H
