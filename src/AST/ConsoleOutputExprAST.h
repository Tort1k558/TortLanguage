#pragma once
#include"AST.h"

class ConsoleOutputExprAST : public ASTNode {
public:
    ConsoleOutputExprAST() = delete;
    ConsoleOutputExprAST(std::shared_ptr<ASTNode> expr)
        : m_expr(expr) {}
    void doSemantic() override;
    void codegen() override;
private:
    std::shared_ptr<ASTNode> m_expr;
};