#pragma once

#include"AST.h"
#include"CompletionInstruction.h"

class ReturnAST : public ASTNode, public CompletionInstruction
{
public:
    ReturnAST() = delete;
    ReturnAST(std::shared_ptr<ASTNode> retExpr)
        : m_returnExpr(retExpr), m_returnBB(nullptr) {}
    void doSemantic() override;
    void codegen() override;
    void setReturnBB(llvm::BasicBlock* BB);
    void setReturnVar(llvm::AllocaInst* var);
    std::shared_ptr<ASTNode> getReturnExpr();
private:
    std::shared_ptr<ASTNode> m_returnExpr;
    llvm::BasicBlock* m_returnBB;
    llvm::AllocaInst* m_returnVar;
};