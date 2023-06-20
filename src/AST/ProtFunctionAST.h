#pragma once

#include"AST.h"
#include"VarDeclAST.h"

class ProtFunctionAST : public ASTNode, public std::enable_shared_from_this<ProtFunctionAST> {
public:
    ProtFunctionAST() = delete;
    ProtFunctionAST(const std::string& name, TokenType retType,
        std::vector<std::shared_ptr<VarDeclAST>> args, std::shared_ptr<SymbolTable> prevSymbolTable)
        : m_returnType(getType(retType)), m_args(args), m_prevSymbolTable(prevSymbolTable)
    {
        setName(name);
    }
    void doSemantic() override;
    void codegen() override;
    std::vector<std::shared_ptr<VarDeclAST>> getArgs();
    llvm::Type* getReturnType();

private:
    llvm::Type* m_returnType;
    std::vector<std::shared_ptr<VarDeclAST>> m_args;
    std::shared_ptr<SymbolTable> m_prevSymbolTable;
};