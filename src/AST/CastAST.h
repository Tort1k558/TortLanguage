#pragma once

#include"AST.h"

class CastAST : public ASTNode
{
public:
    CastAST() = delete;
    CastAST(std::shared_ptr<ASTNode> value, TokenType type)
        : m_value(value), m_type(type) {}
    void doSemantic() override;
    void codegen() override;
private:
    std::shared_ptr<ASTNode> m_value;
    TokenType m_type;
};