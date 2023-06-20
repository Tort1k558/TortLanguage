#pragma once

#include"AST.h"
#include"VarDeclAST.h"

class VarExprAST : public ASTNode {
public:
    VarExprAST() = delete;
    VarExprAST(std::string name) : m_var(nullptr)
    {
        setName(name);
    }

    void doSemantic() override;
    llvm::Value* getRValue() override;
    llvm::Value* getLValue() override;
    void codegen() override;
private:
    std::shared_ptr<VarDeclAST> m_var;
};