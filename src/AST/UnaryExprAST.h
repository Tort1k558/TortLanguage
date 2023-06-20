#pragma once
#include"AST.h"

class UnaryExprAST : public ASTNode {
public:
    UnaryExprAST() = delete;
    UnaryExprAST(TokenType op, const std::string& name, bool prefix = false)
        : m_op(op), m_prefix(prefix)
    {
        setName(name);
    }
    void doSemantic() override;
    void codegen() override;

private:
    TokenType m_op;
    bool m_prefix;
};