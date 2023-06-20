#pragma once

#include"AST.h"
#include"BlockAST.h"

class IfAST : public ASTNode
{
public:
    IfAST() = delete;
    IfAST(std::shared_ptr<ASTNode> ifExpr, std::shared_ptr<BlockAST> ifBlock, std::shared_ptr<BlockAST> elseBlock = nullptr, std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<BlockAST>>> elseIfs = {})
        : m_ifExpr(ifExpr), m_ifBlock(ifBlock), m_elseBlock(elseBlock), m_elseIfs(elseIfs)
    {}
    void doSemantic() override;
    void codegen() override;
    std::shared_ptr<BlockAST> getIfBlock();
    std::shared_ptr<BlockAST> getElseBlock();
    std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<BlockAST>>> getElseIfs();
private:
    std::shared_ptr<ASTNode> m_ifExpr;
    std::shared_ptr<BlockAST> m_ifBlock;
    std::shared_ptr<BlockAST> m_elseBlock;
    std::vector<std::pair<std::shared_ptr<ASTNode>, std::shared_ptr<BlockAST>>> m_elseIfs;

};