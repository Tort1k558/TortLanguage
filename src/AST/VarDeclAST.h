#pragma once

#include"AST.h"

class VarDeclAST : public ASTNode, public std::enable_shared_from_this<VarDeclAST> {
public:
    VarDeclAST() = delete;
    VarDeclAST(const std::string& name, TokenType type, std::shared_ptr<ASTNode> value = nullptr, std::vector<std::shared_ptr<ASTNode>> sizeArray = {}, bool isReference = false)
        :m_value(value), m_type(type), m_sizeArrayAST(sizeArray), m_isReference(isReference)
    {
        setName(name);
    }
    void doSemantic() override;
    llvm::Value* getRValue() override;
    llvm::Value* getLValue() override;
    void codegen() override;

    void setValue(std::shared_ptr<ASTNode> value);
    int getDimensionArray();
    llvm::Type* getContainedType();
    bool isReference();
    std::vector<llvm::Value*> getSizeArrayVLA();
private:
    std::shared_ptr<ASTNode> m_value;
    TokenType m_type;
    llvm::Type* m_containedType;
    std::vector<std::shared_ptr<ASTNode>> m_sizeArrayAST;
    std::vector<llvm::Value*> m_sizeArrayVLA;
    bool m_isReference;
};