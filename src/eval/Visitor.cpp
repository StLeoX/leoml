//
// Created by leo on 2022/4/5.
//


#include "eval/Visitor.h"

template<typename T>
void TreeVisitor<T>::VisitExpbBinary(ExpbBinary *expbBinary) {
#define L TreeVisitor<T>().Eval(expbBinary->_lhs)
#define R TreeVisitor<T>().Eval(expbBinary->_rhs)
    switch (expbBinary->_op) {
        case '+':
            _val = L + R;
            break;
        case '-':
            _val = L - R;
            break;
        case '*':
            _val = L * R;
            break;
        case '/':
            auto l = L, r = R;
            if (r == 0) CompileError(expbBinary->_root, "division by zero");
            _val = l / r;
            break;
        case '<':
            _val = L < R;
            break;
        case '>':
            _val = L > R;
            break;
        case Token::Eq:
            _val = L == R;
            break;
        case Token::Ne:
            _val = L != R;
            break;
        case Token::Le:
            _val = L <= R;
            break;
        case Token::Ge:
            _val = L >= R;
            break;
        default:
            CompileError(expbBinary->_root, "unexpected binary operation");
    }
#undef L
#undef R
}

template<typename T>
void TreeVisitor<T>::VisitExpbUnary(ExpbUnary *expbUnary) {
#define V TreeVisitor<T>().Eval(expbUnary->_oprand)
    switch (expbUnary->_op) {
        case Token::Cons:
            CompilePanic("unimplemented unary operation cons");
            break;
        case Token::Fst:
            CompilePanic("unimplemented unary operation car");
            break;
        case Token::Snd:
            CompilePanic("unimplemented unary operation cdr");
            break;
        default:
            CompileError(expbUnary->_root, "unexpected unary operation");
    }
#undef V
}