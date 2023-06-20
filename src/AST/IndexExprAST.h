#pragma once
#include"AST.h"

class IndexExprAST : public ASTNode
{
public:
    IndexExprAST() = delete;
    IndexExprAST(const std::string& name, std::vector<std::shared_ptr<ASTNode>> indexes, bool getPtr = false)
        : m_indexes(indexes)
    {
        setName(name);
    }
    void doSemantic() override;
    void codegen() override;
    llvm::Value* getRValue() override;
    llvm::Value* getLValue() override;
private:
    std::vector<std::shared_ptr<ASTNode>> m_indexes;
};