#pragma once

#include"AST.h"

class CallExprAST : public ASTNode {
public:
    CallExprAST() = delete;
    CallExprAST(const std::string& name, std::vector<std::shared_ptr<ASTNode>> args)
        : m_args(std::move(args))
    {
        setName(name);
    }
    void doSemantic() override;
    void codegen() override;
    std::string getNameCallFunction();

private:
    std::vector<std::shared_ptr<ASTNode>> m_args;
};