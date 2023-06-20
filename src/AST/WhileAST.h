#pragma once

#include"AST.h"

class WhileAST : public ASTNode
{
public:
    WhileAST() = delete;
    WhileAST(std::shared_ptr<ASTNode> whileExpr, std::shared_ptr<BlockAST> whileBlock)
        : m_whileExpr(whileExpr), m_whileBlock(whileBlock) {}
    void doSemantic() override;
    void codegen() override;
    std::shared_ptr<BlockAST> getWhileBlock();
private:
    std::shared_ptr<ASTNode> m_whileExpr;
    std::shared_ptr<BlockAST> m_whileBlock;
};