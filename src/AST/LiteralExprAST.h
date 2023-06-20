#pragma once
#include"AST.h"

class LiteralExprAST : public ASTNode {
public:
    LiteralExprAST() = delete;
    LiteralExprAST(const std::string& value, TokenType type) : m_value(value), m_type(type) {}

    void doSemantic() override;
    void codegen() override;
    llvm::Value* getRValue() override;
    llvm::Value* getLValue() override;
private:
    std::string m_value;
    TokenType m_type;
};