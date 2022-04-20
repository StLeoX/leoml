//
// Created by leo on 2022/4/4.
//


#include "Parser.h"

/// Parse Float
// ParseConstant aux
ExpaConstant *ParseFloat(const Token *token) {
    const std::string &str = token->str;
    float val = 0.0;
    try {
        val = stof(str);
    } catch (const std::out_of_range &oor) {
        CompileError(token, "float out of range");
    }
    return ExpaConstant::New(token, val);
}

/// Parse Int
// ParseConstant aux
ExpaConstant *ParseInt(const Token *token) {
    const std::string &str = token->str;
    int val = 0;
    try {
        val = stoi(str);
    } catch (const std::out_of_range &oor) {
        CompileError(token, "int out of range");
    }
    return ExpaConstant::New(token, val);
}

ExpaConstant *Parser::ParseConstant(const Token *token) {
    switch (token->tag) {
        case Token::Int:
            return ParseInt(token);
        case Token::Float:
            return ParseFloat(token);
        case Token::String:
            return ExpaConstant::New(token, token->str);
        case Token::Bool:
            return ExpaConstant::New(token, token->str == "true" ? true : false);
        case Token::Unit:
            return ExpaConstant::New(token);
        default:
            CompileError(token, "unexpected contrant");
            return nullptr;
    }
}

Var *Parser::ParseVar(const Token *token) {
    return Var::New(token);
}

ExpaIf *Parser::ParseExpaIf(const Token *token) {
    auto cond = ParseExp();
    _ts.Expect(Token::Then);  // "then" required
    auto then = ParseExp();
    Exp *els = nullptr;
    if (_ts.Try(Token::Else)) {
        els = ParseExp();
    }  // "else" optional
    return ExpaIf::New(token, cond, then, els);
}

ExpaWhile *Parser::ParseExpaWhile(const Token *token) {
    auto cond = ParseExp();
    _ts.Expect(Token::Do);  // "do" required
    auto body = ParseExp();
    _ts.Expect(Token::Done);  // "done" required
    return ExpaWhile::New(token, cond, body);
}

ExpaLet *Parser::ParseExpaLet(const Token *token) {
    auto ret = ExpaLet::New(token);
    do {
        auto assign = ParseAssignStmt(_ts.Peek());
        switch (assign->kind) {
            case Stmt::VarAssignStmt:
                ret->expPairList->push_back(std::pair(assign->var, assign->exp));
                break;
            case Stmt::FuncAssignStmt:
                ret->expPairList->push_back(std::pair(assign->func, static_cast<Exp *>(nullptr)));
                break;
            default:
                CompilePanic("unreachable in ParseExpaLet");
        }
    } while (_ts.Try(Token::And));  // "and" optional, repeated
    _ts.Expect(Token::In);  // "in" required
    ret->body = ParseExp();
    return ret;
}

Expb *Parser::ParseExpaParen(const Token *token) {
    auto exp = Parser::ParseExpb();
    _ts.Expect(Token::RP);
    return exp;
}

Expb *Parser::ParseExpa() {
    auto peek2 = _ts.PeekNext();
    auto peek = _ts.Next();
    if (peek->IsEOF()) CompileError(peek, "premature end of input");
    // exp ::= funcCall
    if (peek->tag == Token::Var && peek2->tag == '(') {
        _ts.PutBack();
        return ParseFuncCall(peek);
    }
    switch (peek->tag) {
        case Token::Var:
            return ParseVar(peek);
        case Token::Int:
        case Token::Float:
        case Token::String:
        case Token::Bool:
        case Token::Unit:
            return ParseConstant(peek);
        case Token::If:
            return ParseExpaIf(peek);
        case Token::While:
            return ParseExpaWhile(peek);
        case Token::Let:
            return ParseExpaLet(peek);
        case Token::LP:  // return expb
            return ParseExpaParen(peek);
        default:
            return nullptr;
    }
    CompileError(peek, "unreachable");
    return nullptr;
}

ExpbBinary *Parser::ParseExpbBinary(const Token *token) {
    auto lhs = ParseExpa();  // pop lhs
    auto rhs = ParseExpbBinaryRHS(0, lhs);  // parse rhs
    auto ret = dynamic_cast<ExpbBinary *>(rhs);  // casting
    ret->TypeCheck();
    return ret;
}

Expb *Parser::ParseExpbBinaryRHS(int prec, Expb *lhs) {
    while (true) {
        auto curToken = _ts.Peek();
        int curPrec = Token::PrecLookup(curToken->tag);
        if (curPrec < prec)
            return lhs;
        auto oper = _ts.Next();  // pop operator
        if (oper->IsEOF()) CompileError(oper, "premature end of input");
        if (!oper->IsBinary()) CompileError(oper, "unexpected binary operator");
        Expb *rhs = ParseExpa();
        if (rhs == nullptr) return nullptr;
        auto nextToken = _ts.Peek();
        int nextPrec = Token::PrecLookup(nextToken->tag);
        if (curPrec < nextPrec) {
            rhs = ParseExpbBinaryRHS(curPrec + 1, rhs);
            if (rhs == nullptr) return nullptr;
        }
        lhs = ExpbBinary::New(curToken, lhs, rhs);
    }
}

ExpbUnary *Parser::ParseExpbUnary(const Token *token) {
    auto oprand = ParseExpb();  // pop oprand
    auto ret = ExpbUnary::New(token, oprand);
    ret->TypeCheck();
    return ret;
}

ExpbCons *Parser::ParseExpbCons(const Token *token) {
    auto first = ParseExpb();
    _ts.Expect(',');
    auto second = ParseExpb();
    _ts.Expect(')');
    return ExpbCons::New(token, first, second);
}

ExpbFst *Parser::ParseExpbFst(const Token *token) {
    _ts.Expect('(');
    auto fisrt = ParseExpb();
    _ts.Expect(',');
    auto second = ParseExpb();
    _ts.Expect(')');
    return ExpbFst::New(token, fisrt, second);
}

ExpbSnd *Parser::ParseExpbSnd(const Token *token) {
    _ts.Expect('(');
    auto fisrt = ParseExpb();
    _ts.Expect(',');
    auto second = ParseExpb();
    _ts.Expect(')');
    return ExpbSnd::New(token, fisrt, second);
}

ExpbCompound *Parser::ParseExpbCompound(const Token *token) {
    auto first = ParseExpa();  // pop first
    _ts.Expect(';');
    auto second = ParseExpb();  // pop second
    return ExpbCompound::New(token, dynamic_cast<Expa *>(first), second);
}

Expb *Parser::ParseExpb() {
    auto peek2 = _ts.PeekNext();
    auto peek = _ts.Next();
    // switch
    if (peek->IsEOF()) CompileError(peek, "premature end of input");
        // Second(expbBinary)
        // LeftRecur, but we can use the peek2 :)
    else if (peek2->IsBinary() || peek2->tag == '(') {
        _ts.PutBack();
        auto before_expa = _ts.Mark();
        auto expa_possible = ParseExpa();
        if (!_ts.Peek()->IsBinary()) { return expa_possible; }
        _ts.ResetTo(before_expa);
        return ParseExpbBinary(peek);
    }
        // Second(expbCompound)
    else if (peek2->tag == Token::Semi) {
        _ts.PutBack();
        return ParseExpbCompound(peek);
    }
        // First(expa)
        // assert(!peek2->IsBinary());
    else if (peek->tag == Token::Var || peek->IsConstant() || peek->tag == Token::If || peek->tag == Token::While ||
             peek->tag == Token::Let) {
        _ts.PutBack();
        return ParseExpa();
    }
        // First(expbUnary)
    else if (peek->IsUnary()) { return ParseExpbUnary(peek); }
        // First(expbCons)
    else if (peek->tag == Token::LP) { return ParseExpbCons(peek); }
        // First(expbFst)
    else if (peek->tag == Token::Fst) { return ParseExpbFst(peek); }
        // First(expbCons)
    else if (peek->tag == Token::Snd) { return ParseExpbSnd(peek); }
        // default return nullptr as condition
    else {
        return nullptr;
    }
    CompilePanic("unreachable");
    return nullptr;
}

Exp *Parser::ParseExp() {
    auto peek2 = _ts.PeekNext(); //* Next() will influent the PeekNext(), so get peek2 first.
    auto peek = _ts.Next(); //* Use Next to get the peek.
    auto ret = Exp::New(peek);
    if (peek->IsEOF()) CompileError(peek, "premature end of input");
        // exp ::= var expblist
    else if (peek->tag == Token::Var && !peek2->IsBinary() && peek2->tag != '(') {
        ret->var = ParseVar(peek);
        auto expb = ParseExpb();
        while (expb != nullptr) { // not nullptr
            ret->expbList->push_back(expb);
            expb = ParseExpb();
        }
        _ts.PutBack();
        return ret;
    }
        // exp ::= expb
    else {
        _ts.PutBack();
        ret->expbList->push_back(ParseExpb());
        return ret;
    }
    return nullptr;
}

Func *Parser::ParseFunc(const Token *token) {
    auto ret = Func::New(token);
    _ts.Try(Token::Rec);  // todo: "rec" related operation.
    ret->name = ParseVar(_ts.Next())->name;
    if (_ts.Test('(')) {
        _ts.Next();
        do {
            ret->paramList->push_back(ParseVar(_ts.Next()));
        } while (_ts.Try(Token::Comma));
        _ts.Expect(')');
    }
    return ret;
}

FuncCall *Parser::ParseFuncCall(const Token *token) {
    auto ret = FuncCall::New(token);
    ret->name = ParseVar(_ts.Next())->name;
    if (_ts.Test('(')) {
        _ts.Next();
        do {
            ret->argList->push_back(ParseExpb());
        } while (_ts.Try(Token::Comma));
        _ts.Expect(')');
    }
    ret->paramList = nullptr;
    ret->retValue = nullptr; // Unknown retValue before evaluating.
    return ret;
}

Stmt *Parser::ParseAssignStmt(const Token *token) {
    auto ret = Stmt::New();
    auto peek3 = _ts.PeekNext();
    // var assign kind
    if (peek3->tag == '=') {
        ret->kind = Stmt::VarAssignStmt;
        ret->var = ParseVar(_ts.Next());
        _ts.Expect('=');
        ret->exp = ParseExp();
        return ret;
    } // func assign kind
    else {
        ret->kind = Stmt::FuncAssignStmt;
        auto funcStmt = ParseFunc(token);
        _ts.Expect('=');
        funcStmt->body = ParseExp();
        ret->func = funcStmt;
        return ret;
    }
}

Stmt *Parser::ParseStmt() {
    auto peek = _ts.Next();
    // let... stmt kind
    if (peek->tag == Token::Let) {
        auto ret = ParseAssignStmt(peek);
        _ts.Expect(Token::Dsemi);
        return ret;
    }
        // var stmt kind
    else if (peek->tag == Token::Var) {
        auto ret = Stmt::New();
        ret->kind = Stmt::VarStmt;
        ret->var = ParseVar(peek);
        _ts.Expect(Token::Dsemi);
        return ret;
    } else {
        CompileError(peek, "unexpected stmt start");
        return nullptr;
    }
    CompileError(peek, "unreachable");
    return nullptr;
}

Program *Parser::ParseProgram() {
    auto ret = Program::New();
    while (!_ts.Peek()->IsEOF()) {
        ret->stmtList->push_back(ParseStmt());
    }
    return ret;
}

void Parser::Serialize(std::ostream &os) {
    _program->Serialize(os);
}
