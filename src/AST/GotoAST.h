#pragma once

#include"AST.h"

class GotoAST : public ASTNode
{
public:
    GotoAST() = delete;
    GotoAST(llvm::BasicBlock* gotoBB, llvm::BasicBlock* gotoElseBB = nullptr, llvm::Value* value = nullptr)
        : m_gotoBB(gotoBB), m_gotoelseBB(gotoElseBB), m_value(m_value) {}
    void doSemantic() override;
    void codegen() override;
private:
    llvm::BasicBlock* m_gotoBB;
    llvm::BasicBlock* m_gotoelseBB;
    llvm::Value* m_value;
};