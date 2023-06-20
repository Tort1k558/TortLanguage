#pragma once
#include"AST.h"

class LLVMCallFuncAST : public ASTNode
{
public:
    LLVMCallFuncAST() = delete;
    LLVMCallFuncAST(llvm::Function* function, llvm::ArrayRef<llvm::Value*> args = std::nullopt)
        : m_function(function), m_args(args)
    {}
    void doSemantic() override;
    void codegen() override;
private:
    llvm::Function* m_function;
    std::vector<llvm::Value*> m_args;
};