#pragma once
#include"AST.h"

class LLVMValueAST : public ASTNode
{
public:
    LLVMValueAST() = delete;
    LLVMValueAST(llvm::Value* value) : m_value(value)
    {}
    void doSemantic() override;
    void codegen() override;

private:
    llvm::Value* m_value;
};