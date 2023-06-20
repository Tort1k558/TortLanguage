#pragma once
#include"AST.h"

class BinaryExprAST : public ASTNode {
public:
    BinaryExprAST() = delete;
    BinaryExprAST(TokenType op, std::shared_ptr<ASTNode> lhs, std::shared_ptr<ASTNode> rhs)
        : m_op(op), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}
    void doSemantic() override;
    void codegen() override;
private:
    TokenType m_op;
    std::shared_ptr<ASTNode> m_lhs, m_rhs;
};